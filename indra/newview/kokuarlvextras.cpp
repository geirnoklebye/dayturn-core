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

#include "llerror.h"
#include "llsdserialize.h" 

#include "kokuarlvextras.h"


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

std::string KokuaRLVExtras::getDummyName (std::string name)
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