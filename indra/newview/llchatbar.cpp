/** 
 * @file llchatbar.cpp
 * @brief LLChatBar class implementation
 *
 * $LicenseInfo:firstyear=2002&license=viewerlgpl$
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

//MK
#include "linden_common.h"
//mk

#include "llviewerprecompiledheaders.h"

#include "llchatbar.h"

#include "imageids.h"
#include "llfontgl.h"
#include "llrect.h"
#include "llerror.h"
#include "llparcel.h"
#include "llstring.h"
#include "message.h"
#include "llfocusmgr.h"

#include "llagent.h"
#include "llbutton.h"
#include "llcombobox.h"
#include "llcommandhandler.h"	// secondlife:///app/chat/ support
#include "llviewercontrol.h"
#include "llgesturemgr.h"
#include "llkeyboard.h"
#include "lllineeditor.h"
#include "llstatusbar.h"
#include "lltextbox.h"
#include "lluiconstants.h"
#include "llviewergesture.h"			// for triggering gestures
#include "llviewermenu.h"		// for deleting object with DEL key
#include "llviewerstats.h"
#include "llviewerwindow.h"
#include "llframetimer.h"
#include "llresmgr.h"
#include "llworld.h"
#include "llinventorymodel.h"
#include "llmultigesture.h"
#include "llui.h"
#include "llviewermenu.h"
#include "lluictrlfactory.h"

//
// Globals
//
const F32 AGENT_TYPING_TIMEOUT = 5.f;	// seconds

LLChatBar *gChatBar = NULL;

class LLChatBarGestureObserver : public LLGestureManagerObserver
{
public:
	LLChatBarGestureObserver(LLChatBar* chat_barp) : mChatBar(chat_barp){}
	virtual ~LLChatBarGestureObserver() {}
	virtual void changed() { mChatBar->refreshGestures(); }
private:
	LLChatBar* mChatBar;
};


extern void send_chat_from_viewer(const std::string& utf8_out_text, EChatType type, S32 channel);

//
// Functions
//

LLChatBar::LLChatBar() 
:	LLPanel(),
	mInputEditor(NULL),
	mGestureLabelTimer(),
	mLastSpecialChatChannel(0),
	mIsBuilt(FALSE),
	mGestureCombo(NULL),
	mObserver(NULL)
{
	//setIsChrome(TRUE);
}


LLChatBar::~LLChatBar()
{
	LLGestureMgr::instance().removeObserver(mObserver);
	delete mObserver;
	mObserver = NULL;
	// LLView destructor cleans up children
}

BOOL LLChatBar::postBuild()
{
	getChild<LLUICtrl>("Say")->setCommitCallback(boost::bind(&LLChatBar::onClickSay, this, _1));

	// * NOTE: mantipov: getChild with default parameters returns dummy widget.
	// Seems this class will be completle removed
	// attempt to bind to an existing combo box named gesture
	setGestureCombo(findChild<LLComboBox>( "Gesture"));

	mInputEditor = getChild<LLLineEditor>("Chat Editor");
	mInputEditor->setKeystrokeCallback(&onInputEditorKeystroke, this);
	mInputEditor->setFocusLostCallback(boost::bind(&LLChatBar::onInputEditorFocusLost));
	mInputEditor->setFocusReceivedCallback(boost::bind(&LLChatBar::onInputEditorGainFocus));
	mInputEditor->setCommitOnFocusLost( FALSE );
	mInputEditor->setRevertOnEsc( FALSE );
	mInputEditor->setIgnoreTab(TRUE);
	mInputEditor->setPassDelete(TRUE);
	mInputEditor->setReplaceNewlinesWithSpaces(FALSE);

	mInputEditor->setMaxTextLength(DB_CHAT_MSG_STR_LEN);
	mInputEditor->setEnableLineHistory(TRUE);

	mIsBuilt = TRUE;

	return TRUE;
}

//-----------------------------------------------------------------------
// Overrides
//-----------------------------------------------------------------------

// virtual
BOOL LLChatBar::handleKeyHere( KEY key, MASK mask )
{
	BOOL handled = FALSE;

	if( KEY_RETURN == key )
	{
		if (mask == MASK_CONTROL)
		{
			// shout
			sendChat(CHAT_TYPE_SHOUT);
			handled = TRUE;
		}
//MK
		else if (mask == MASK_SHIFT)
		{
			// whisper
			sendChat( CHAT_TYPE_WHISPER );
			handled = TRUE;
		}
//mk
		else if (mask == MASK_NONE)
		{
			// say
			sendChat( CHAT_TYPE_NORMAL );
			handled = TRUE;
		}
	}
	// only do this in main chatbar
	else if ( KEY_ESCAPE == key && gChatBar == this)
	{
		stopChat();

		handled = TRUE;
	}

	return handled;
}

void LLChatBar::refresh()
{
	// HACK: Leave the name of the gesture in place for a few seconds.
	const F32 SHOW_GESTURE_NAME_TIME = 2.f;
	if (mGestureLabelTimer.getStarted() && mGestureLabelTimer.getElapsedTimeF32() > SHOW_GESTURE_NAME_TIME)
	{
		LLCtrlListInterface* gestures = mGestureCombo ? mGestureCombo->getListInterface() : NULL;
		if (gestures) gestures->selectFirstItem();
		mGestureLabelTimer.stop();
	}

	if ((gAgent.getTypingTime() > AGENT_TYPING_TIMEOUT) && (gAgent.getRenderState() & AGENT_STATE_TYPING))
	{
		gAgent.stopTyping();
	}

	getChildView("Say")->setEnabled(mInputEditor->getText().size() > 0);

}

void LLChatBar::refreshGestures()
{
	if (mGestureCombo)
	{
		//store current selection so we can maintain it
		std::string cur_gesture = mGestureCombo->getValue().asString();
		mGestureCombo->selectFirstItem();
		std::string label = mGestureCombo->getValue().asString();;
		// clear
		mGestureCombo->clearRows();

		// collect list of unique gestures
		std::map <std::string, BOOL> unique;
		LLGestureMgr::item_map_t::const_iterator it;
		const LLGestureMgr::item_map_t& active_gestures = LLGestureMgr::instance().getActiveGestures();
		for (it = active_gestures.begin(); it != active_gestures.end(); ++it)
		{
			LLMultiGesture* gesture = (*it).second;
			if (gesture)
			{
				if (!gesture->mTrigger.empty())
				{
					unique[gesture->mTrigger] = TRUE;
				}
			}
		}

		// add unique gestures
		std::map <std::string, BOOL>::iterator it2;
		for (it2 = unique.begin(); it2 != unique.end(); ++it2)
		{
			mGestureCombo->addSimpleElement((*it2).first);
		}
		
		mGestureCombo->sortByName();
		// Insert label after sorting, at top, with separator below it
		mGestureCombo->addSeparator(ADD_TOP);
		mGestureCombo->addSimpleElement(getString("gesture_label"), ADD_TOP);
		
		if (!cur_gesture.empty())
		{ 
			mGestureCombo->selectByValue(LLSD(cur_gesture));
		}
		else
		{
			mGestureCombo->selectFirstItem();
		}
	}
}

// Move the cursor to the correct input field.
void LLChatBar::setKeyboardFocus(BOOL focus)
{
	if (focus)
	{
		if (mInputEditor)
		{
			mInputEditor->setFocus(TRUE);
			mInputEditor->selectAll();
		}
	}
	else if (gFocusMgr.childHasKeyboardFocus(this))
	{
		if (mInputEditor)
		{
			mInputEditor->deselect();
		}
		setFocus(FALSE);
	}
}


// Ignore arrow keys in chat bar
void LLChatBar::setIgnoreArrowKeys(BOOL b)
{
	if (mInputEditor)
	{
		mInputEditor->setIgnoreArrowKeys(b);
	}
}

BOOL LLChatBar::inputEditorHasFocus()
{
	return mInputEditor && mInputEditor->hasFocus();
}

std::string LLChatBar::getCurrentChat()
{
	return mInputEditor ? mInputEditor->getText() : LLStringUtil::null;
}

void LLChatBar::setGestureCombo(LLComboBox* combo)
{
	mGestureCombo = combo;
	if (mGestureCombo)
	{
		mGestureCombo->setCommitCallback(boost::bind(&LLChatBar::onCommitGesture, this, _1));

		// now register observer since we have a place to put the results
		mObserver = new LLChatBarGestureObserver(this);
		LLGestureMgr::instance().addObserver(mObserver);

		// refresh list from current active gestures
		refreshGestures();
	}
}

//-----------------------------------------------------------------------
// Internal functions
//-----------------------------------------------------------------------

// If input of the form "/20foo" or "/20 foo", returns "foo" and channel 20.
// Otherwise returns input and channel 0.
LLWString LLChatBar::stripChannelNumber(const LLWString &mesg, S32* channel)
{
	if (mesg[0] == '/'
		&& mesg[1] == '/')
	{
		// This is a "repeat channel send"
		*channel = mLastSpecialChatChannel;
		return mesg.substr(2, mesg.length() - 2);
	}
	else if (mesg[0] == '/'
			 && mesg[1]
			 && LLStringOps::isDigit(mesg[1]))
	{
		// This a special "/20" speak on a channel
		S32 pos = 0;

		// Copy the channel number into a string
		LLWString channel_string;
		llwchar c;
		do
		{
			c = mesg[pos+1];
			channel_string.push_back(c);
			pos++;
		}
		while(c && pos < 64 && LLStringOps::isDigit(c));
		
		// Move the pointer forward to the first non-whitespace char
		// Check isspace before looping, so we can handle "/33foo"
		// as well as "/33 foo"
		while(c && iswspace(c))
		{
			c = mesg[pos+1];
			pos++;
		}
		
		mLastSpecialChatChannel = strtol(wstring_to_utf8str(channel_string).c_str(), NULL, 10);
		*channel = mLastSpecialChatChannel;
		return mesg.substr(pos, mesg.length() - pos);
	}
	else
	{
		// This is normal chat.
		*channel = 0;
		return mesg;
	}
}


void LLChatBar::sendChat( EChatType type )
{
	if (mInputEditor)
	{
		LLWString text = mInputEditor->getConvertedText();
		if (!text.empty())
		{
			// store sent line in history, duplicates will get filtered
			if (mInputEditor) mInputEditor->updateHistory();
			// Check if this is destined for another channel
			S32 channel = 0;
			stripChannelNumber(text, &channel);
			
			std::string utf8text = wstring_to_utf8str(text);
			// Try to trigger a gesture, if not chat to a script.
			std::string utf8_revised_text;
			if (0 == channel)
			{
//-TT Patch MU_OOC from Satomi Ahn
//				if (gSavedSettings.getBOOL("AutoCloseOOC"))
				{
					// Try to find any unclosed OOC chat (i.e. an opening
					// double parenthesis without a matching closing double
					// parenthesis.
					if (utf8text.find("(( ") != -1 && utf8text.find("))") == -1)
					{
						// add the missing closing double parenthesis.
						utf8text += " ))";
					}
					else if (utf8text.find("((") != -1 && utf8text.find("))") == -1)
					{
						if (utf8text.at(utf8text.length() - 1) == ')')
						{
							// cosmetic: add a space first to avoid a closing triple parenthesis
							utf8text += " ";
						}
						// add the missing closing double parenthesis.
						utf8text += "))";
					}
					else if (utf8text.find("[[ ") != -1 && utf8text.find("]]") == -1)
					{
						// add the missing closing double parenthesis.
						utf8text += " ]]";
					}
					else if (utf8text.find("[[") != -1 && utf8text.find("]]") == -1)
					{
						if (utf8text.at(utf8text.length() - 1) == ']')
						{
							// cosmetic: add a space first to avoid a closing triple parenthesis
							utf8text += " ";
						}
						// add the missing closing double parenthesis.
						utf8text += "]]";
					}
				}

				// Convert MU*s style poses into IRC emotes here.
				if (gSavedSettings.getBOOL("AllowMUpose"))
				{
					if (utf8text.find(":") == 0 && utf8text.length() > 3)
					{
						if (utf8text.find(":'") == 0)
						{
							utf8text.replace(0, 1, "/me");
	 					}
	 					else if (isalpha(utf8text.at(1)))	// Do not prevent smileys and such.
						{
							utf8text.replace(0, 1, "/me ");
						}
					}
				}
//-TT Patch MU_OOC from Satomi Ahn
//MK
////			// discard returned "found" boolean
////				LLGestureMgr::instance().triggerAndReviseString(utf8text, &utf8_revised_text);
				BOOL found_gesture=LLGestureMgr::instance().triggerAndReviseString(utf8text, &utf8_revised_text);

				if (gRRenabled && gAgent.mRRInterface.contains ("sendchat") && !gAgent.mRRInterface.containsSubstr ("redirchat:"))
				{
					// user is forbidden to send any chat message on channel 0 except emotes and OOC text
					utf8_revised_text = gAgent.mRRInterface.crunchEmote (utf8_revised_text, 20);
					if (found_gesture && utf8_revised_text=="...") utf8_revised_text="";
				}
//mk
			}
			else
			{
//MK
				std::ostringstream stream;
				stream << "" << channel;
				if (gRRenabled && gAgent.mRRInterface.containsWithoutException ("sendchannel", stream.str()))
				{
					utf8_revised_text = "";
				}
				else
//mk
				utf8_revised_text = utf8text;
			}

			utf8_revised_text = utf8str_trim(utf8_revised_text);

			if (!utf8_revised_text.empty())
			{
				// Chat with animation
				sendChatFromViewer(utf8_revised_text, type, TRUE);
			}
		}
	}

	getChild<LLUICtrl>("Chat Editor")->setValue(LLStringUtil::null);

	gAgent.stopTyping();

	// If the user wants to stop chatting on hitting return, lose focus
	// and go out of chat mode.
	if (gChatBar == this && gSavedSettings.getBOOL("CloseChatOnReturn"))
	{
		stopChat();
	}
}


//-----------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------

// static 
void LLChatBar::startChat(const char* line)
{
	//TODO* remove DUMMY chat
	//if(gBottomTray && gBottomTray->getChatBox())
	//{
	//	gBottomTray->setVisible(TRUE);
	//	gBottomTray->getChatBox()->setFocus(TRUE);
	//}

	// *TODO Vadim: Why was this code commented out?

// 	gChatBar->setVisible(TRUE);
// 	gChatBar->setKeyboardFocus(TRUE);
// 	gSavedSettings.setBOOL("ChatVisible", TRUE);
// 
// 	if (line && gChatBar->mInputEditor)
// 	{
// 		std::string line_string(line);
// 		gChatBar->mInputEditor->setText(line_string);
// 	}
// 	// always move cursor to end so users don't obliterate chat when accidentally hitting WASD
// 	gChatBar->mInputEditor->setCursorToEnd();
}


// Exit "chat mode" and do the appropriate focus changes
// static
void LLChatBar::stopChat()
{
	//TODO* remove DUMMY chat
	//if(gBottomTray && gBottomTray->getChatBox())
	///{
	//	gBottomTray->getChatBox()->setFocus(FALSE);
	//}

	// *TODO Vadim: Why was this code commented out?

// 	// In simple UI mode, we never release focus from the chat bar
// 	gChatBar->setKeyboardFocus(FALSE);
// 
// 	// If we typed a movement key and pressed return during the
// 	// same frame, the keyboard handlers will see the key as having
// 	// gone down this frame and try to move the avatar.
// 	gKeyboard->resetKeys();
// 	gKeyboard->resetMaskKeys();
// 
// 	// stop typing animation
// 	gAgent.stopTyping();
// 
// 	// hide chat bar so it doesn't grab focus back
// 	gChatBar->setVisible(FALSE);
// 	gSavedSettings.setBOOL("ChatVisible", FALSE);
}

// static
void LLChatBar::onInputEditorKeystroke( LLLineEditor* caller, void* userdata )
{
	LLChatBar* self = (LLChatBar *)userdata;

	LLWString raw_text;
	if (self->mInputEditor) raw_text = self->mInputEditor->getWText();

	// Can't trim the end, because that will cause autocompletion
	// to eat trailing spaces that might be part of a gesture.
	LLWStringUtil::trimHead(raw_text);

	S32 length = raw_text.length();

	if( (length > 0) && (raw_text[0] != '/') )  // forward slash is used for escape (eg. emote) sequences
	{
//MK
		if (!gRRenabled || !gAgent.mRRInterface.containsSubstr ("redirchat:"))
//mk
			gAgent.startTyping();
	}
	else
	{
		gAgent.stopTyping();
	}

	/* Doesn't work -- can't tell the difference between a backspace
	   that killed the selection vs. backspace at the end of line.
	if (length > 1 
		&& text[0] == '/'
		&& key == KEY_BACKSPACE)
	{
		// the selection will already be deleted, but we need to trim
		// off the character before
		std::string new_text = raw_text.substr(0, length-1);
		self->mInputEditor->setText( new_text );
		self->mInputEditor->setCursorToEnd();
		length = length - 1;
	}
	*/

	KEY key = gKeyboard->currentKey();

	// Ignore "special" keys, like backspace, arrows, etc.
	if (length > 1 
		&& raw_text[0] == '/'
		&& key < KEY_SPECIAL)
	{
		// we're starting a gesture, attempt to autocomplete

		std::string utf8_trigger = wstring_to_utf8str(raw_text);
		std::string utf8_out_str(utf8_trigger);

		if (LLGestureMgr::instance().matchPrefix(utf8_trigger, &utf8_out_str))
		{
			if (self->mInputEditor)
			{
				std::string rest_of_match = utf8_out_str.substr(utf8_trigger.size());
				self->mInputEditor->setText(utf8_trigger + rest_of_match); // keep original capitalization for user-entered part
				S32 outlength = self->mInputEditor->getLength(); // in characters
			
				// Select to end of line, starting from the character
				// after the last one the user typed.
				self->mInputEditor->setSelection(length, outlength);
			}
		}

		//llinfos << "GESTUREDEBUG " << trigger 
		//	<< " len " << length
		//	<< " outlen " << out_str.getLength()
		//	<< llendl;
	}
}

// static
void LLChatBar::onInputEditorFocusLost()
{
	// stop typing animation
	gAgent.stopTyping();
}

// static
void LLChatBar::onInputEditorGainFocus()
{
	//LLFloaterChat::setHistoryCursorAndScrollToEnd();
}

void LLChatBar::onClickSay( LLUICtrl* ctrl )
{
	std::string cmd = ctrl->getValue().asString();
	e_chat_type chat_type = CHAT_TYPE_NORMAL;
	if (cmd == "shout")
	{
		chat_type = CHAT_TYPE_SHOUT;
	}
	else if (cmd == "whisper")
	{
		chat_type = CHAT_TYPE_WHISPER;
	}
	sendChat(chat_type);
}

void LLChatBar::sendChatFromViewer(const std::string &utf8text, EChatType type, BOOL animate)
{
	sendChatFromViewer(utf8str_to_wstring(utf8text), type, animate);
}

void LLChatBar::sendChatFromViewer(const LLWString &wtext, EChatType type, BOOL animate)
{
	// as soon as we say something, we no longer care about teaching the user
	// how to chat
	gWarningSettings.setBOOL("FirstOtherChatBeforeUser", FALSE);
	
	// Look for "/20 foo" channel chats.
	S32 channel = 0;
	LLWString out_text = stripChannelNumber(wtext, &channel);
	std::string utf8_out_text = wstring_to_utf8str(out_text);
	if (!utf8_out_text.empty())
	{
		utf8_out_text = utf8str_truncate(utf8_out_text, MAX_MSG_STR_LEN);
	}

	std::string utf8_text = wstring_to_utf8str(wtext);
	utf8_text = utf8str_trim(utf8_text);
	if (!utf8_text.empty())
	{
		utf8_text = utf8str_truncate(utf8_text, MAX_STRING - 1);
	}

//MK
	if (gRRenabled && channel == 0)
	{
		// transform the type according to chatshout, chatnormal and chatwhisper restrictions
		if (type == CHAT_TYPE_WHISPER && gAgent.mRRInterface.contains ("chatwhisper"))
		{
			type = CHAT_TYPE_NORMAL;
		}
		if (type == CHAT_TYPE_SHOUT && gAgent.mRRInterface.contains ("chatshout"))
		{
			type = CHAT_TYPE_NORMAL;
		}
		if ((type == CHAT_TYPE_SHOUT || type == CHAT_TYPE_NORMAL)
			&& gAgent.mRRInterface.contains ("chatnormal"))
		{
			type = CHAT_TYPE_WHISPER;
		}
		
		if (gAgent.mRRInterface.containsSubstr ("redirchat:"))
		{
			animate = false;
		}
	}
//mk
	// Don't animate for chats people can't hear (chat to scripts)
	if (animate && (channel == 0))
	{
		if (type == CHAT_TYPE_WHISPER)
		{
			lldebugs << "You whisper " << utf8_text << llendl;
			gAgent.sendAnimationRequest(ANIM_AGENT_WHISPER, ANIM_REQUEST_START);
		}
		else if (type == CHAT_TYPE_NORMAL)
		{
			lldebugs << "You say " << utf8_text << llendl;
			gAgent.sendAnimationRequest(ANIM_AGENT_TALK, ANIM_REQUEST_START);
		}
		else if (type == CHAT_TYPE_SHOUT)
		{
			lldebugs << "You shout " << utf8_text << llendl;
			gAgent.sendAnimationRequest(ANIM_AGENT_SHOUT, ANIM_REQUEST_START);
		}
		else
		{
			llinfos << "send_chat_from_viewer() - invalid volume" << llendl;
			return;
		}
	}
	else
	{
		if (type != CHAT_TYPE_START && type != CHAT_TYPE_STOP)
		{
			lldebugs << "Channel chat: " << utf8_text << llendl;
		}
	}

	send_chat_from_viewer(utf8_out_text, type, channel);
}
/*
void send_chat_from_viewer(const std::string& utf8_out_text, EChatType type, S32 channel)
{
//MK
	if (gRRenabled && channel >= 2147483647 && gAgent.mRRInterface.contains ("sendchat"))
	{
		// When prevented from talking, remove the ability to talk on the DEBUG_CHANNEL altogether, since it is a way of cheating
		return;
	}
	
	if (gRRenabled && channel == 0)
	{
		std::string restriction;

		// We might want to redirect this chat or emote (and exit this function early on)
		if (utf8_out_text.find ("/me ") == 0 // emote
			|| utf8_out_text.find ("/me's") == 0) // emote
		{
			if (gAgent.mRRInterface.containsSubstr ("rediremote:"))
			{
				restriction = "rediremote:";
			}
		}
		else if (utf8_out_text.find ("((") != 0 || utf8_out_text.find ("))") != utf8_out_text.length () - 2)
		{
			if (gAgent.mRRInterface.containsSubstr ("redirchat:"))
			{
				restriction = "redirchat:";
			}
		}

		if (!restriction.empty())
		{
			// Public chat or emote redirected => for each redirection, send the same message on the target channel
			RRMAP::iterator it = gAgent.mRRInterface.mSpecialObjectBehaviours.begin ();
			std::string behav;
			while (it != gAgent.mRRInterface.mSpecialObjectBehaviours.end())
			{
				behav = it->second;
				if (behav.find (restriction) == 0)
				{
					S32 ch = atoi (behav.substr (restriction.length()).c_str());
					std::ostringstream stream;
					stream << "" << ch;
					if (!gAgent.mRRInterface.containsWithoutException ("sendchannel", stream.str()))
					{
						if (ch > 0 && ch < 2147483647)
						{
							LLMessageSystem* msg = gMessageSystem;
							msg->newMessageFast(_PREHASH_ChatFromViewer);
							msg->nextBlockFast(_PREHASH_AgentData);
							msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
							msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
							msg->nextBlockFast(_PREHASH_ChatData);
							msg->addStringFast(_PREHASH_Message, utf8_out_text);
							msg->addU8Fast(_PREHASH_Type, type);
							msg->addS32("Channel", ch);

							gAgent.sendReliableMessage();
						}
					}
				}
				it++;
			}

			LLViewerStats::getInstance()->incStat(LLViewerStats::ST_CHAT_COUNT);

			// We have redirected the chat message, don't send it on the original channel
			return;
		}
	}

	std::string crunchedText = utf8_out_text;
	
	// There is a redirection in order but this particular message is an emote or an OOC text, so we didn't
	// redirect it. However it has not gone through crunchEmote yet, so we need to do this here to prevent
	// cheated, emote-like chat (true emotes must however go through untouched).
	if (gRRenabled && channel == 0 && gAgent.mRRInterface.containsSubstr ("redirchat:"))
	{
		crunchedText = gAgent.mRRInterface.crunchEmote(crunchedText);
	}
//mk
	
	LLMessageSystem* msg = gMessageSystem;
	msg->newMessageFast(_PREHASH_ChatFromViewer);
	msg->nextBlockFast(_PREHASH_AgentData);
	msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
	msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
	msg->nextBlockFast(_PREHASH_ChatData);
////	msg->addStringFast(_PREHASH_Message, utf8_out_text);
//MK	
	msg->addStringFast(_PREHASH_Message, crunchedText);
//mk
	msg->addU8Fast(_PREHASH_Type, type);
	msg->addS32("Channel", channel);

	gAgent.sendReliableMessage();

	LLViewerStats::getInstance()->incStat(LLViewerStats::ST_CHAT_COUNT);
}
*/

void LLChatBar::onCommitGesture(LLUICtrl* ctrl)
{
	LLCtrlListInterface* gestures = mGestureCombo ? mGestureCombo->getListInterface() : NULL;
	if (gestures)
	{
		S32 index = gestures->getFirstSelectedIndex();
		if (index == 0)
		{
			return;
		}
		const std::string& trigger = gestures->getSelectedValue().asString();

//MK
		if (!gRRenabled || !gAgent.mRRInterface.contains ("sendchat"))
		{
//mk
			// pretend the user chatted the trigger string, to invoke
			// substitution and logging.
			std::string text(trigger);
			std::string revised_text;
			LLGestureMgr::instance().triggerAndReviseString(text, &revised_text);

			revised_text = utf8str_trim(revised_text);
			if (!revised_text.empty())
			{
				// Don't play nodding animation
				sendChatFromViewer(revised_text, CHAT_TYPE_NORMAL, FALSE);
			}
//MK
		}
//mk
	}
	mGestureLabelTimer.start();
	if (mGestureCombo != NULL)
	{
		// free focus back to chat bar
		mGestureCombo->setFocus(FALSE);
	}
}
