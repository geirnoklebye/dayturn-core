/** 
 * @file kokuarlvextras.cpp
 * @brief Additional RLV code separate from RRInterface
 *
 * $LicenseInfo:firstyear=2006&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2010, Linden Research, Inc.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
 * $/LicenseInfo$
 */

#include "llviewerprecompiledheaders.h"
#include <map>

#include "kokuarlvextras.h"

#include "llagent.h"
#include "llagentcamera.h"
#include "llavatarnamecache.h"
#include "llerror.h"
#include "llsdserialize.h" 
#include "llworld.h"
#include "RRInterface.h"



// This file is intended for Kokua RLV features where including them in RRInterface would make it
// difficult to merge as well as making an already large source file even larger. Its initial
// purpose is to house the new routine for more intelligent choice of anonymised names however
// it's foreseen that other code will go in here over time.

kokua_rlv_extras_handler_t gKokuaRLVExtrasHandler;

std::string KokuaRLVExtras::m_StringMapPath;
 
struct embedded_anonym_t
{
	std::string mOriginalName;
	U32 mLastReferenced;
};
typedef std::map<std::string, embedded_anonym_t > anonym_map_t;
anonym_map_t mAnonEntries; 
 
// ---- all initialisation, called from llStartup just after other RLV setup

void KokuaRLVExtras::initialise()
{
	// the file loading is adapted from Kitty Barnett's RLVa code
	static bool fInitialized = false;
	if (!fInitialized)
	{
		KokuaRLVExtras::getInstance()->mLaunchTimestamp = LLDate::now().secondsSinceEpoch();
		// Load the default string values
		std::vector<std::string> files = gDirUtilp->findSkinnedFilenames(LLDir::XUI, KOKUA_RLV_NAMES_FILE, LLDir::ALL_SKINS);
		m_StringMapPath = (!files.empty()) ? files.front() : LLStringUtil::null;
		for (std::vector<std::string>::const_iterator itFile = files.begin(); itFile != files.end(); ++itFile)
		{
			loadFromFile(*itFile);
		}

		// Sanity check
		if ( (mAnonEntries.empty()) )
		{
			LL_ERRS() << "Problem parsing RLV names XML file" << LL_ENDL;
			return;
		}

		fInitialized = true;
	}	
}

// this is also adapted from Kitty's routine, however we're only using it to bring in names since
// the rest of RLV is inherently not internationalisable. We also change from vector to map
// for storage since we need added data associated with each anonym
void KokuaRLVExtras::loadFromFile(const std::string& strFilePath)
{
	llifstream fileStream(strFilePath.c_str(), std::ios::binary); LLSD sdFileData;
	if ( (!fileStream.is_open()) || (!LLSDSerialize::fromXMLDocument(sdFileData, fileStream)) )
		return;
	fileStream.close();

	if (sdFileData.has("anonyms"))
	{
		const LLSD& sdAnonyms = sdFileData["anonyms"];
		U32 initialStamp = LLDate::now().secondsSinceEpoch();
		for (LLSD::array_const_iterator itAnonym = sdAnonyms.beginArray(); itAnonym != sdAnonyms.endArray(); ++itAnonym)
		{
			struct embedded_anonym_t new_anon;
			new_anon.mOriginalName = "";
			new_anon.mLastReferenced = initialStamp;
			mAnonEntries[(*itAnonym).asString()] = new_anon;
		}
	}
}

std::string KokuaRLVExtras::kokuaGetDummyName (std::string name)
{
	// KKA-646
	// RLV and RLVa's original routines both use a hash algorithm for picking the anonym to associate with
	// a name. The repeatability is needed so that mapping is consistent during a session however it has
	// the downside that it can happen there are only two avatars around and they both get given the same
	// anonym. This routine is slightly smarter and ensures double assignment won't happen by accident.
	// The approach taken is to assign an item with the oldest access time, discarding the previous use
	// if necessary
	//
	// However, it can happen that there are too many names in close proximity. To avoid cache thrashing
	// and the likelihood of anon names changes as a consequence we fall back to a hash-based selection
	// if we find that all our timestamps are the same
	
	anonym_map_t::iterator oldest = mAnonEntries.end();
	anonym_map_t::iterator matched = mAnonEntries.end();
	U32 oldest_stamp = -1; //ie maximum value
	U32 oldest_count = 0;
	anonym_map_t::iterator itAnonym = mAnonEntries.begin();
		
	while (itAnonym != mAnonEntries.end() && matched == mAnonEntries.end())
	{
		if ((itAnonym->second).mOriginalName == name) matched = itAnonym;
		if ((itAnonym->second).mLastReferenced == oldest_stamp)	oldest_count++;
		if ((itAnonym->second).mLastReferenced < oldest_stamp)
		{
			oldest = itAnonym;
			oldest_stamp = (itAnonym->second).mLastReferenced;
			oldest_count = 1;
		}
		itAnonym++;
	}
	
	if (matched != mAnonEntries.end())
	{
		// LL_INFOS() << "matched " << name << " to " << (matched->second).mOriginalName << " as " << matched->first<< LL_ENDL;
		(matched->second).mLastReferenced = LLDate::now().secondsSinceEpoch();
		return matched->first;
	}
	
	if (oldest_count == mAnonEntries.size())
	{
		// We're in a pathological situation where there are more nearby names than we have anonyms for.
		// Fall back to hash-based determination of which entry to reuse so that we can avoid the effect
		// of avatars changing anonyms as they fall out of the cache and come back in. For this we're using
		// Kitty's algorithm but with Marine's nudge based on the login time for session to session variance
		
		const char* pszName = name.c_str();
		U32 nHash = 0;
		anonym_map_t::iterator nthitem = mAnonEntries.begin();
		U32 nthcount = 0;
		
		for (int idx = 0, cnt = name.length(); idx < cnt; idx++)
		{
			nHash += pszName[idx];
		}
			
		nHash = nHash + (mLaunchTimestamp >> 16); // add some session to session variability per Marine's implementation

		nHash = nHash % mAnonEntries.size();
	  
		// there doesn't seem to be an easy way to pull the Nth item of a map without walking through, so...
		
		while (nthcount < nHash)
		{
			nthitem++;
			nthcount++;
		}
		
		// LL_INFOS() << "Overflow situation, chose hash item " << nHash << LL_ENDL;

		(nthitem->second).mLastReferenced = LLDate::now().secondsSinceEpoch();
		(nthitem->second).mOriginalName = name;
		return nthitem->first;	
	}
	
	// LL_INFOS() << "Not found, reusing " << oldest->first << " occupied by " << (oldest->second).mOriginalName << LL_ENDL;
	(oldest->second).mLastReferenced = LLDate::now().secondsSinceEpoch();
	(oldest->second).mOriginalName = name;
	return oldest->first;	
}

std::string KokuaRLVExtras::kokuaGetCensoredMessage(std::string str, bool anon_name)
{
	// HACK: if the message is under the form secondlife:///app/agent/UUID/about, clear it
	// (we could just as well clear just the first part, or return a bogus message, 
	// the purpose here is to avoid showing the profile of an avatar and displaying their name on the chat)
	if (str.find("secondlife:///app/agent/") == 0)
	{
		return "";
	}

	// First we need to build a list of all the exceptions to @shownames and @shownametags
	// Each avatar object which UUID is contained in this list should not be censored
	std::string command;
	std::string behav;
	std::string option;
	std::string param;
	std::deque<LLUUID> exceptions;
	RRMAP::iterator it = gAgent.mRRInterface.mSpecialObjectBehaviours.begin();
	while (it != gAgent.mRRInterface.mSpecialObjectBehaviours.end()) {
		command = it->second;
		LLStringUtil::toLower(command);
		if (command.find("shownames:") == 0 || command.find("shownames_sec:") == 0 || command.find("shownametags:") == 0) {
			// KKA-635 this needs the =n to work
			if (gAgent.mRRInterface.parseCommand(command+"=n", behav, option, param)) {
				LLUUID uuid;
				uuid.set(option, FALSE);
				exceptions.push_back(uuid);
			}
		}
		it++;
	}
	
	// KKA-638 Previously this searched the object cache to find avatars which apart from being an intensive
	// operation had the problem that avatars within parcels with visibility turned off don't appear in the 
	// object cache. Instead, we now use the same method used by the minimap to get its avatar list
	
	std::string pre_str = str;
	uuid_vec_t avatar_ids;
	std::vector<LLVector3d> positions;
	
	LLWorld::getInstance()->getAvatars(&avatar_ids, &positions, gAgentCamera.getCameraPositionGlobal());

	for (U32 i = 0; i < avatar_ids.size(); i++)
	{
		LLUUID uuid = avatar_ids[i];

				LLAvatarName av_name;
				if (LLAvatarNameCache::get(uuid, &av_name))
				{
					std::string user_name = av_name.getUserName();
					std::string clean_user_name = LLCacheName::cleanFullName(user_name);
					std::string  display_name = av_name.mDisplayName; // not "getDisplayName()" because we need this whether we use display names or user names
					//KKA-631 we need to handle first.last as well
					std::string  user_dot_name = av_name.mUsername; // needed for minimap and maybe others now they come through here

					if (std::find(exceptions.begin(), exceptions.end(), uuid) == exceptions.end()) // ignore exceptions
					{
						//KKA-630 to reduce the occurrences of same avatar different names in different situations (eg chat, tooltip etc) this
						//is tweaked slightly to always derive the dummy name from user_name. getDummyName is changed to veneer into this function
						//so that exceptions can be handled whilst kokuaGetDummyName is the original routine
						std::string  dummy_name = kokuaGetDummyName(user_name);
							
						//KKA-658 when called via getDummyName don't do substrings, require an exact match to do anything
						if (anon_name)
						{
							if (str == clean_user_name + " Resident"
								|| str == clean_user_name
								|| str == user_name + " Resident"
								|| str == user_name
								|| str == display_name + " Resident"
								|| str == display_name
								|| str == user_dot_name)
							{
								str = dummy_name;
								anon_name = false;
								break;
							}
						}
						else
						{
							//KKA-658 if we get an exact name match on the input string (not the evolving substituted string)
							//substitute that and stop at that point						
							if (pre_str == clean_user_name || pre_str == clean_user_name + " Resident" || pre_str == user_name || pre_str == user_name + " Resident"
							|| pre_str==display_name || pre_str==display_name + " Resident" || pre_str == user_dot_name)
							{
								str = dummy_name;
								break;
							}
							//KKA-658 we're still going to have problems in cases where two names could appear in free text and one
							//is a wholly contained subset of the other (eg display names 0025 and 0025a). The test above deals with
							//this if the input string is only a name, but when there's other text as well I don't see an easy solution
							//without getting into sorting all the possible substitutions by length so we guarantee trying the longest first
							//That's more processing than I want to be doing in this routine
							
							if (user_name.find(" ") == -1) str = gAgent.mRRInterface.stringReplaceWholeWord(str, clean_user_name + " Resident", dummy_name);
							str = gAgent.mRRInterface.stringReplaceWholeWord(str, clean_user_name, dummy_name);

							if (user_name.find(" ") == -1) str = gAgent.mRRInterface.stringReplaceWholeWord(str, user_name + " Resident", dummy_name);
							str = gAgent.mRRInterface.stringReplaceWholeWord(str, user_name, dummy_name);

							if (user_name.find(" ") == -1) str = gAgent.mRRInterface.stringReplaceWholeWord(str, display_name + " Resident", dummy_name);
							str = gAgent.mRRInterface.stringReplaceWholeWord(str, display_name, dummy_name);
							
							//KKA-631 handle first.last for minimap
							str = gAgent.mRRInterface.stringReplaceWholeWord(str, user_dot_name, dummy_name);
						}
					}
					else
					{
						// KKA-630 this av has an exception - if str exactly matches one of the name options turn off anon_name to preserve it
						// KKA-658 change this test to be on the original input string so that we can correctly handle the case where one name
						// is wholly contained in a larger name and the larger name has an exception. Test anon_name too to allow a bit of optimisation
						if (anon_name && (pre_str == clean_user_name || pre_str == clean_user_name + " Resident" || pre_str == user_name || pre_str == user_name + " Resident"
							|| pre_str==display_name || pre_str==display_name + " Resident" || pre_str == user_dot_name))
						{
							str = pre_str; // undo any partial substitutions that could have happened up to this point
							anon_name = false;
							break;
						}
					}
				}
	}
	//KKA-630 If we were called to anonymise the whole string (ie it's known to be a name) and we didn't find an exception, anonymise it
	//This isn't ideal - references to non-present avatars with an exception are going to get anonymised, however it's only in cases where
	//anonymisation is being forced
	if (str.compare(pre_str) == 0 && anon_name) str = kokuaGetDummyName(str);
	return str;
}
