/** 
 * @file llpanelavatar.cpp
 * @brief LLPanelAvatar and related class implementations
 *
 * $LicenseInfo:firstyear=2004&license=viewerlgpl$
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
#include "llpanelavatar.h"

#include "llagent.h"
#include "llavataractions.h"
#include "llavatarconstants.h"	// AVATAR_ONLINE
#include "llcallingcard.h"
#include "llcombobox.h"
#include "lldateutil.h"			// ageFromDate()
#include "llimview.h"
#include "llmenubutton.h"
#include "llnotificationsutil.h"
#include "llslurl.h"
#include "lltexteditor.h"
#include "lltexturectrl.h"
#include "lltoggleablemenu.h"
#include "lltooldraganddrop.h"
#include "llscrollcontainer.h"
#include "llavatariconctrl.h"
#include "llfloaterreg.h"
#include "llnotificationsutil.h"
#include "llvoiceclient.h"
#include "lltextbox.h"
#include "lltrans.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class LLDropTarget
//
// This handy class is a simple way to drop something on another
// view. It handles drop events, always setting itself to the size of
// its parent.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class LLDropTarget : public LLView
{
public:
	struct Params : public LLInitParam::Block<Params, LLView::Params>
	{
		Optional<LLUUID> agent_id;
		Params()
		:	agent_id("agent_id")
		{
			changeDefault(mouse_opaque, false);
			changeDefault(follows.flags, FOLLOWS_ALL);
		}
	};

	LLDropTarget(const Params&);
	~LLDropTarget();

	void doDrop(EDragAndDropType cargo_type, void* cargo_data);

	//
	// LLView functionality
	virtual BOOL handleDragAndDrop(S32 x, S32 y, MASK mask, BOOL drop,
								   EDragAndDropType cargo_type,
								   void* cargo_data,
								   EAcceptance* accept,
								   std::string& tooltip_msg);
	void setAgentID(const LLUUID &agent_id)		{ mAgentID = agent_id; }
protected:
	LLUUID mAgentID;
};

LLDropTarget::LLDropTarget(const LLDropTarget::Params& p) 
:	LLView(p),
	mAgentID(p.agent_id)
{}

LLDropTarget::~LLDropTarget()
{}

void LLDropTarget::doDrop(EDragAndDropType cargo_type, void* cargo_data)
{
	llinfos << "LLDropTarget::doDrop()" << llendl;
}

BOOL LLDropTarget::handleDragAndDrop(S32 x, S32 y, MASK mask, BOOL drop,
									 EDragAndDropType cargo_type,
									 void* cargo_data,
									 EAcceptance* accept,
									 std::string& tooltip_msg)
{
	if(getParent())
	{
		LLToolDragAndDrop::handleGiveDragAndDrop(mAgentID, LLUUID::null, drop,
												 cargo_type, cargo_data, accept);

		return TRUE;
	}

	return FALSE;
}

static LLDefaultChildRegistry::Register<LLDropTarget> r("drop_target");

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

LLPanelProfileTab::LLPanelProfileTab()
: LLPanel()
, mAvatarId(LLUUID::null)
{
}

LLPanelProfileTab::~LLPanelProfileTab()
{
	if(getAvatarId().notNull())
	{
		LLAvatarPropertiesProcessor::getInstance()->removeObserver(getAvatarId(),this);
	}
}

void LLPanelProfileTab::setAvatarId(const LLUUID& id)
{
	if(id.notNull())
	{
		if(getAvatarId().notNull())
		{
			LLAvatarPropertiesProcessor::getInstance()->removeObserver(mAvatarId,this);
		}
		mAvatarId = id;
		LLAvatarPropertiesProcessor::getInstance()->addObserver(getAvatarId(),this);
	}
}

void LLPanelProfileTab::onOpen(const LLSD& key)
{
	// Don't reset panel if we are opening it for same avatar.
	if(getAvatarId() != key.asUUID())
	{
		resetControls();
		resetData();

		scrollToTop();
	}

	// Update data even if we are viewing same avatar profile as some data might been changed.
	setAvatarId(key.asUUID());
	updateData();
	updateButtons();
}

void LLPanelProfileTab::scrollToTop()
{
	LLScrollContainer* scrollContainer = findChild<LLScrollContainer>("profile_scroll");
	if (scrollContainer)
		scrollContainer->goToTop();
}

void LLPanelProfileTab::onMapButtonClick()
{
	LLAvatarActions::showOnMap(getAvatarId());
}

void LLPanelProfileTab::updateButtons()
{
	bool is_buddy_online = LLAvatarTracker::instance().isBuddyOnline(getAvatarId());
	
	if(LLAvatarActions::isFriend(getAvatarId()))
	{
		getChildView("teleport")->setEnabled(is_buddy_online);
	}
	else
	{
		getChildView("teleport")->setEnabled(true);
	}

	bool enable_map_btn = (is_buddy_online &&
			       is_agent_mappable(getAvatarId()))
		|| gAgent.isGodlike();
	getChildView("show_on_map_btn")->setEnabled(enable_map_btn);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool enable_god()
{
	return gAgent.isGodlike();
}

LLPanelAvatarProfile::LLPanelAvatarProfile()
: LLPanelProfileTab()
{
}

BOOL LLPanelAvatarProfile::postBuild()
{
	childSetCommitCallback("add_friend",(boost::bind(&LLPanelAvatarProfile::onAddFriendButtonClick,this)),NULL);
	childSetCommitCallback("im",(boost::bind(&LLPanelAvatarProfile::onIMButtonClick,this)),NULL);
	childSetCommitCallback("call",(boost::bind(&LLPanelAvatarProfile::onCallButtonClick,this)),NULL);
	childSetCommitCallback("teleport",(boost::bind(&LLPanelAvatarProfile::onTeleportButtonClick,this)),NULL);
	childSetCommitCallback("overflow_btn", boost::bind(&LLPanelAvatarProfile::onOverflowButtonClicked, this), NULL);
	childSetCommitCallback("share",(boost::bind(&LLPanelAvatarProfile::onShareButtonClick,this)),NULL);
	childSetCommitCallback("show_on_map_btn", (boost::bind(
			&LLPanelAvatarProfile::onMapButtonClick, this)), NULL);

	LLUICtrl::CommitCallbackRegistry::ScopedRegistrar registrar;
	registrar.add("Profile.ShowOnMap",  boost::bind(&LLPanelAvatarProfile::onMapButtonClick, this));
	registrar.add("Profile.Pay",  boost::bind(&LLPanelAvatarProfile::pay, this));
	registrar.add("Profile.Share", boost::bind(&LLPanelAvatarProfile::share, this));
	registrar.add("Profile.BlockUnblock", boost::bind(&LLPanelAvatarProfile::toggleBlock, this));
	registrar.add("Profile.Kick", boost::bind(&LLPanelAvatarProfile::kick, this));
	registrar.add("Profile.Freeze", boost::bind(&LLPanelAvatarProfile::freeze, this));
	registrar.add("Profile.Unfreeze", boost::bind(&LLPanelAvatarProfile::unfreeze, this));
	registrar.add("Profile.CSR", boost::bind(&LLPanelAvatarProfile::csr, this));

	LLUICtrl::EnableCallbackRegistry::ScopedRegistrar enable;
	enable.add("Profile.EnableShowOnMap", boost::bind(&LLPanelAvatarProfile::enableShowOnMap, this));
	enable.add("Profile.EnableGod", boost::bind(&enable_god));
	enable.add("Profile.EnableBlock", boost::bind(&LLPanelAvatarProfile::enableBlock, this));
	enable.add("Profile.EnableUnblock", boost::bind(&LLPanelAvatarProfile::enableUnblock, this));

	mProfileMenu = LLUICtrlFactory::getInstance()->createFromFile<LLToggleableMenu>("menu_profile_overflow.xml", gMenuHolder, LLViewerMenuHolderGL::child_registry_t::instance());

	LLVoiceClient::getInstance()->addObserver((LLVoiceClientStatusObserver*)this);

	resetControls();
	resetData();

	return TRUE;
}

void LLPanelAvatarProfile::onOpen(const LLSD& key)
{
	LLPanelProfileTab::onOpen(key);

	mGroups.clear();

	//Disable "Add Friend" button for friends.
	getChildView("add_friend")->setEnabled(!LLAvatarActions::isFriend(getAvatarId()));
}

void LLPanelAvatarProfile::updateData()
{
	if (getAvatarId().notNull())
	{
		LLAvatarPropertiesProcessor::getInstance()->
			sendAvatarPropertiesRequest(getAvatarId());
		LLAvatarPropertiesProcessor::getInstance()->
			sendAvatarGroupsRequest(getAvatarId());
	}
}

void LLPanelAvatarProfile::resetControls()
{
	getChildView("status_panel")->setVisible( true);
	getChildView("profile_buttons_panel")->setVisible( true);
	getChildView("title_groups_text")->setVisible( true);
	getChildView("sl_groups")->setVisible( true);
	getChildView("add_friend")->setEnabled(true);

	getChildView("status_me_panel")->setVisible( false);
	getChildView("profile_me_buttons_panel")->setVisible( false);
	getChildView("account_actions_panel")->setVisible( false);
}

void LLPanelAvatarProfile::resetData()
{
	mGroups.clear();
	getChild<LLUICtrl>("2nd_life_pic")->setValue(LLUUID::null);
	getChild<LLUICtrl>("real_world_pic")->setValue(LLUUID::null);
	getChild<LLUICtrl>("online_status")->setValue(LLStringUtil::null);
	getChild<LLUICtrl>("status_message")->setValue(LLStringUtil::null);
	getChild<LLUICtrl>("sl_description_edit")->setValue(LLStringUtil::null);
	getChild<LLUICtrl>("fl_description_edit")->setValue(LLStringUtil::null);
	getChild<LLUICtrl>("sl_groups")->setValue(LLStringUtil::null);
	getChild<LLUICtrl>("homepage_edit")->setValue(LLStringUtil::null);
	getChild<LLUICtrl>("register_date")->setValue(LLStringUtil::null);
	getChild<LLUICtrl>("acc_status_text")->setValue(LLStringUtil::null);
	getChild<LLUICtrl>("partner_text")->setValue(LLStringUtil::null);
}

void LLPanelAvatarProfile::processProperties(void* data, EAvatarProcessorType type)
{
	if(APT_PROPERTIES == type)
	{
		const LLAvatarData* avatar_data = static_cast<const LLAvatarData*>(data);
		if(avatar_data && getAvatarId() == avatar_data->avatar_id)
		{
			processProfileProperties(avatar_data);
		}
	}
	else if(APT_GROUPS == type)
	{
		LLAvatarGroups* avatar_groups = static_cast<LLAvatarGroups*>(data);
		if(avatar_groups && getAvatarId() == avatar_groups->avatar_id)
		{
			processGroupProperties(avatar_groups);
		}
	}
}

void LLPanelAvatarProfile::processProfileProperties(const LLAvatarData* avatar_data)
{
	fillCommonData(avatar_data);

	fillPartnerData(avatar_data);

	fillAccountStatus(avatar_data);
}

void LLPanelAvatarProfile::processGroupProperties(const LLAvatarGroups* avatar_groups)
{
	// *NOTE dzaporozhan
	// Group properties may arrive in two callbacks, we need to save them across
	// different calls. We can't do that in textbox as textbox may change the text.

	LLAvatarGroups::group_list_t::const_iterator it = avatar_groups->group_list.begin();
	const LLAvatarGroups::group_list_t::const_iterator it_end = avatar_groups->group_list.end();

	for(; it_end != it; ++it)
	{
		LLAvatarGroups::LLGroupData group_data = *it;
		mGroups[group_data.group_name] = group_data.group_id;
	}

	// Creating string, containing group list
	std::string groups = "";
	for (group_map_t::iterator it = mGroups.begin(); it != mGroups.end(); ++it)
	{
		if (it != mGroups.begin())
			groups += ", ";

		std::string group_name = LLURI::escape(it->first);
		std::string group_url= it->second.notNull()
				? "[hop:///app/group/" + it->second.asString() + "/about " + group_name + "]"
						: getString("no_group_text");

		groups += group_url;
	}

	getChild<LLUICtrl>("sl_groups")->setValue(groups);
}

void LLPanelAvatarProfile::fillCommonData(const LLAvatarData* avatar_data)
{
	//remove avatar id from cache to get fresh info
	LLAvatarIconIDCache::getInstance()->remove(avatar_data->avatar_id);

	LLStringUtil::format_map_t args;
	{
		std::string birth_date = LLTrans::getString("AvatarBirthDateFormat");
		LLStringUtil::format(birth_date, LLSD().with("datetime", (S32) avatar_data->born_on.secondsSinceEpoch()));
		args["[REG_DATE]"] = birth_date;
	}
	args["[AGE]"] = LLDateUtil::ageFromDate( avatar_data->born_on, LLDate::now());
	std::string register_date = getString("RegisterDateFormat", args);
	getChild<LLUICtrl>("register_date")->setValue(register_date );
	getChild<LLUICtrl>("sl_description_edit")->setValue(avatar_data->about_text);
	getChild<LLUICtrl>("fl_description_edit")->setValue(avatar_data->fl_about_text);
	getChild<LLUICtrl>("2nd_life_pic")->setValue(avatar_data->image_id);
	getChild<LLUICtrl>("real_world_pic")->setValue(avatar_data->fl_image_id);
	getChild<LLUICtrl>("homepage_edit")->setValue(avatar_data->profile_url);

	// Hide home page textbox if no page was set to fix "homepage URL appears clickable without URL - EXT-4734"
	getChildView("homepage_edit")->setVisible( !avatar_data->profile_url.empty());
}

void LLPanelAvatarProfile::fillPartnerData(const LLAvatarData* avatar_data)
{
	LLTextBox* partner_text = getChild<LLTextBox>("partner_text");
	if (avatar_data->partner_id.notNull())
	{
		partner_text->setText(LLSLURL("agent", avatar_data->partner_id, "inspect").getSLURLString());
	}
	else
	{
		partner_text->setText(getString("no_partner_text"));
	}
}

void LLPanelAvatarProfile::fillAccountStatus(const LLAvatarData* avatar_data)
{
	LLStringUtil::format_map_t args;
	args["[ACCTTYPE]"] = LLAvatarPropertiesProcessor::accountType(avatar_data);
	args["[PAYMENTINFO]"] = LLAvatarPropertiesProcessor::paymentInfo(avatar_data);
	// *NOTE: AVATAR_AGEVERIFIED not currently getting set in 
	// dataserver/lldataavatar.cpp for privacy considerations
	args["[AGEVERIFICATION]"] = "";
	std::string caption_text = getString("CaptionTextAcctInfo", args);
	getChild<LLUICtrl>("acc_status_text")->setValue(caption_text);
}

void LLPanelAvatarProfile::pay()
{
	LLAvatarActions::pay(getAvatarId());
}

void LLPanelAvatarProfile::share()
{
	LLAvatarActions::share(getAvatarId());
}

void LLPanelAvatarProfile::toggleBlock()
{
	LLAvatarActions::toggleBlock(getAvatarId());
}

bool LLPanelAvatarProfile::enableShowOnMap()
{
	bool is_buddy_online = LLAvatarTracker::instance().isBuddyOnline(getAvatarId());

	bool enable_map_btn = (is_buddy_online && is_agent_mappable(getAvatarId()))
		|| gAgent.isGodlike();
	return enable_map_btn;
}

bool LLPanelAvatarProfile::enableBlock()
{
	return LLAvatarActions::canBlock(getAvatarId()) && !LLAvatarActions::isBlocked(getAvatarId());
}

bool LLPanelAvatarProfile::enableUnblock()
{
	return LLAvatarActions::isBlocked(getAvatarId());
}

void LLPanelAvatarProfile::kick()
{
	LLAvatarActions::kick(getAvatarId());
}

void LLPanelAvatarProfile::freeze()
{
	LLAvatarActions::freeze(getAvatarId());
}

void LLPanelAvatarProfile::unfreeze()
{
	LLAvatarActions::unfreeze(getAvatarId());
}

void LLPanelAvatarProfile::csr()
{
	std::string name;
	gCacheName->getFullName(getAvatarId(), name);
	LLAvatarActions::csr(getAvatarId(), name);
}

void LLPanelAvatarProfile::onAddFriendButtonClick()
{
	LLAvatarActions::requestFriendshipDialog(getAvatarId());
}

void LLPanelAvatarProfile::onIMButtonClick()
{
	LLAvatarActions::startIM(getAvatarId());
}

void LLPanelAvatarProfile::onTeleportButtonClick()
{
	LLAvatarActions::offerTeleport(getAvatarId());
}

void LLPanelAvatarProfile::onCallButtonClick()
{
	LLAvatarActions::startCall(getAvatarId());
}

void LLPanelAvatarProfile::onShareButtonClick()
{
	//*TODO not implemented
}

void LLPanelAvatarProfile::onOverflowButtonClicked()
{
	if (!mProfileMenu->toggleVisibility())
		return;

	LLView* btn = getChild<LLView>("overflow_btn");

	if (mProfileMenu->getButtonRect().isEmpty())
	{
		mProfileMenu->setButtonRect(btn);
	}
	mProfileMenu->updateParent(LLMenuGL::sMenuContainer);

	LLRect rect = btn->getRect();
	LLMenuGL::showPopup(this, mProfileMenu, rect.mRight, rect.mTop);
}

LLPanelAvatarProfile::~LLPanelAvatarProfile()
{
	if(getAvatarId().notNull())
	{
		LLAvatarTracker::instance().removeParticularFriendObserver(getAvatarId(), this);
		if(LLVoiceClient::instanceExists())
		{
			LLVoiceClient::getInstance()->removeObserver((LLVoiceClientStatusObserver*)this);
		}
	}
}

// virtual, called by LLAvatarTracker
void LLPanelAvatarProfile::changed(U32 mask)
{
	getChildView("teleport")->setEnabled(LLAvatarTracker::instance().isBuddyOnline(getAvatarId()));
}

// virtual
void LLPanelAvatarProfile::onChange(EStatusType status, const std::string &channelURI, bool proximal)
{
	if(status == STATUS_JOINING || status == STATUS_LEFT_CHANNEL)
	{
		return;
	}

	getChildView("call")->setEnabled(LLVoiceClient::getInstance()->voiceEnabled() && LLVoiceClient::getInstance()->isVoiceWorking());
}

void LLPanelAvatarProfile::setAvatarId(const LLUUID& id)
{
	if(id.notNull())
	{
		if(getAvatarId().notNull())
		{
			LLAvatarTracker::instance().removeParticularFriendObserver(getAvatarId(), this);
		}
		LLPanelProfileTab::setAvatarId(id);
		LLAvatarTracker::instance().addParticularFriendObserver(getAvatarId(), this);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

LLPanelMyProfile::LLPanelMyProfile()
: LLPanelAvatarProfile()
{
}

BOOL LLPanelMyProfile::postBuild()
{
	LLPanelAvatarProfile::postBuild();

	childSetCommitCallback("status_me_message_text", boost::bind(&LLPanelMyProfile::onStatusMessageChanged, this), NULL);

	resetControls();
	resetData();

	return TRUE;
}

void LLPanelMyProfile::onOpen(const LLSD& key)
{
	LLPanelProfileTab::onOpen(key);
}

void LLPanelMyProfile::processProfileProperties(const LLAvatarData* avatar_data)
{
	fillCommonData(avatar_data);

	fillPartnerData(avatar_data);

	fillAccountStatus(avatar_data);
}

void LLPanelMyProfile::resetControls()
{
	getChildView("status_panel")->setVisible( false);
	getChildView("profile_buttons_panel")->setVisible( false);
	getChildView("title_groups_text")->setVisible( false);
	getChildView("sl_groups")->setVisible( false);
	getChildView("status_me_panel")->setVisible( true);
	getChildView("profile_me_buttons_panel")->setVisible( true);
}


void LLPanelMyProfile::onStatusMessageChanged()
{
	updateData();
}
