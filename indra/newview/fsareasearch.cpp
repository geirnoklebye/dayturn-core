/* Copyright (c) 2009
 *
 * Modular Systems Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *   3. Neither the name Modular Systems Ltd nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS LTD AND CONTRIBUTORS “AS IS”
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MODULAR SYSTEMS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Modified, debugged, optimized and improved by Henri Beauchamp Feb 2010.
 * Refactored for Viewer2 by Kadah Coba, April 2011
 */

#include "llviewerprecompiledheaders.h"

#include "fsareasearch.h"

#include "llagent.h"
#include "llblocklist.h"
#include "llfloaterreg.h"
#include "llfloaterreporter.h"
#include "lllineeditor.h"
#include "llmutelist.h"
#include "llpanelblockedlist.h"
#include "llscrolllistctrl.h"
#include "lltextbox.h"
#include "lltoolgrab.h"
#include "lltracker.h"
#include "llviewercontrol.h"
#include "llviewermenu.h"
#include "llviewerobjectlist.h"
#include "llviewerparcelmgr.h"
#include "llviewerwindow.h"

#include <boost/algorithm/string/find.hpp> //for boost::ifind_first

const std::string request_string = "FSAreaSearch::Requested_ø§µ";
const F32 min_refresh_interval = 0.25f;	// Minimum interval between list refreshes in seconds.


class FSAreaSearch::FSParcelChangeObserver : public LLParcelObserver
{
public:
	FSParcelChangeObserver(FSAreaSearch* area_search_floater) : mAreaSearchFloater(area_search_floater) {}

private:
	/*virtual*/ void changed()
	{
		if (mAreaSearchFloater)
		{
			mAreaSearchFloater->checkRegion();
		}
	}

	FSAreaSearch* mAreaSearchFloater;
};

FSAreaSearch::FSAreaSearch(const LLSD& key) :  
LLFloater(key),
mCounterText(0),
mResultList(0)
{
	mLastUpdateTimer.reset();
}

FSAreaSearch::~FSAreaSearch()
{
	if (mParcelChangedObserver)
	{
		LLViewerParcelMgr::getInstance()->removeObserver(mParcelChangedObserver);
		delete mParcelChangedObserver;
	}
}

BOOL FSAreaSearch::postBuild()
{
	mResultList = getChild<LLScrollListCtrl>("result_list");

	if (mResultList) {
		mResultList->setDoubleClickCallback(boost::bind(&FSAreaSearch::onDoubleClick, this));
		mResultList->setRightMouseDownCallback(boost::bind(&FSAreaSearch::onRightClick, this, _2, _3));
		mResultList->sortByColumn("Name", TRUE);
	}

	mCounterText = getChild<LLTextBox>("counter");

	childSetAction("Refresh", boost::bind(&FSAreaSearch::search, this));
	childSetAction("Stop", boost::bind(&FSAreaSearch::cancel, this));
	
	getChild<LLLineEditor>("Name query chunk")->setKeystrokeCallback( boost::bind(&FSAreaSearch::onCommitLine, this, _1, _2),NULL);
	getChild<LLLineEditor>("Description query chunk")->setKeystrokeCallback( boost::bind(&FSAreaSearch::onCommitLine, this, _1, _2),NULL);
	getChild<LLLineEditor>("Owner query chunk")->setKeystrokeCallback( boost::bind(&FSAreaSearch::onCommitLine, this, _1, _2),NULL);
	getChild<LLLineEditor>("Group query chunk")->setKeystrokeCallback( boost::bind(&FSAreaSearch::onCommitLine, this, _1, _2),NULL);

	mParcelChangedObserver = new FSParcelChangeObserver(this);
	LLViewerParcelMgr::getInstance()->addObserver(mParcelChangedObserver);

	return TRUE;
}

void FSAreaSearch::checkRegion()
{
	// Check if we changed region, and if we did, clear the object details cache.
	LLViewerRegion* region = gAgent.getRegion();
	if (region != mLastRegion)
	{
		mLastRegion = region;
		mRequested = 0;
		mObjectDetails.clear();
		
		mResultList->deleteAllItems();
		mCounterText->setText(getString("ListedPendingTotalBlank"));
	}
}

void FSAreaSearch::onDoubleClick()
{
 	LLScrollListItem *item = mResultList->getFirstSelected();
	if (!item) {
		return;
	}

	LLUUID id = item->getUUID();

	LLViewerObject *object = gObjectList.findObject(id);

	if (object) {
		LLTracker::trackLocation(object->getPositionGlobal(), mObjectDetails[id].name, "", LLTracker::LOCATION_ITEM);
	}
}

void FSAreaSearch::onRightClick(S32 x, S32 y)
{
	LLScrollListItem *item = mResultList->hitItem(x, y);

	if (!item) {
		return;
	}

	LLUUID id = item->getUUID();

	if (id.notNull()) {
		uuid_vec_t uuids;
		uuids.push_back(id);

		mResultList->selectByID(id);

		LLViewerObject *object = gObjectList.findObject(id);

		if (object) {
			LLSelectMgr::getInstance()->deselectAll();
			mSelectionHandle = LLSelectMgr::getInstance()->selectObjectAndFamily(object);
		}

		ContextMenu::instance().show(mResultList, uuids, x, y);
	}
}

void FSAreaSearch::cancel()
{
	checkRegion();
	closeFloater();
	
	mSearchedName = "";
	mSearchedDesc = "";
	mSearchedOwner = "";
	mSearchedGroup = "";
}

void FSAreaSearch::search()
{
	checkRegion();
	results();
}

void FSAreaSearch::onCommitLine(LLLineEditor* line, void* user_data)
{
	std::string name = line->getName();
	std::string text = line->getText();

	if (name == "Name query chunk") mSearchedName = text;
	else if (name == "Description query chunk") mSearchedDesc = text;
	else if (name == "Owner query chunk") mSearchedOwner = text;
	else if (name == "Group query chunk") mSearchedGroup = text;

	if (text.length() > 3)
	{
		checkRegion();
		results();
	}
}

void FSAreaSearch::requestIfNeeded(LLViewerObject *objectp)
{
	LLUUID object_id = objectp->getID();
	if (mObjectDetails.count(object_id) == 0)
	{
		ObjectDetails *details = &mObjectDetails[object_id];
		details->name = request_string;
		details->desc = request_string;
		details->owner_id = LLUUID::null;
		details->group_id = LLUUID::null;

		LLMessageSystem* msg = gMessageSystem;
		msg->newMessageFast(_PREHASH_RequestObjectPropertiesFamily);
		msg->nextBlockFast(_PREHASH_AgentData);
		msg->addUUIDFast(_PREHASH_AgentID, gAgent.getID());
		msg->addUUIDFast(_PREHASH_SessionID, gAgent.getSessionID());
		msg->nextBlockFast(_PREHASH_ObjectData);
		msg->addU32Fast(_PREHASH_RequestFlags, 0 );
		msg->addUUIDFast(_PREHASH_ObjectID, object_id);
		gAgent.sendReliableMessage();
		mRequested++;
	}
}

void FSAreaSearch::results()
{
	if (!getVisible()) return;
	if (mRequested > 0 && mLastUpdateTimer.getElapsedTimeF32() < min_refresh_interval) return;

	const LLUUID selected = mResultList->getCurrentID();
	const S32 scrollpos = mResultList->getScrollPos();
	mResultList->deleteAllItems();

	S32 i;
	S32 total = gObjectList.getNumObjects();
	LLViewerRegion* our_region = gAgent.getRegion();
	for (i = 0; i < total; i++)
	{
		LLViewerObject *objectp = gObjectList.getObject(i);
		if (objectp)
		{
			if (objectp->getRegion() == our_region && !objectp->isAvatar() && objectp->isRoot() &&
				!objectp->flagTemporary() && !objectp->flagTemporaryOnRez())
			{
				LLUUID object_id = objectp->getID();
				if (mObjectDetails.count(object_id) == 0)
				{
					requestIfNeeded(objectp);
				}
				else
				{
					ObjectDetails *details = &mObjectDetails[object_id];
					std::string object_name = details->name;
					std::string object_desc = details->desc;
					std::string object_owner;
					std::string object_group;
					gCacheName->getFullName(details->owner_id, object_owner);
					gCacheName->getGroupName(details->group_id, object_group);
					if (object_name != request_string)
					{
						if ((mSearchedName == "" || boost::ifind_first(object_name, mSearchedName)) &&
							(mSearchedDesc == "" || boost::ifind_first(object_desc, mSearchedDesc)) &&
							(mSearchedOwner == "" || boost::ifind_first(object_owner, mSearchedOwner)) &&
							(mSearchedGroup == "" || boost::ifind_first(object_group, mSearchedGroup)))
						{
							LLSD element;
							element["id"] = object_id;
							element["columns"][LIST_OBJECT_NAME]["column"] = "Name";
							element["columns"][LIST_OBJECT_NAME]["type"] = "text";
							element["columns"][LIST_OBJECT_NAME]["value"] = details->name;
							element["columns"][LIST_OBJECT_DESC]["column"] = "Description";
							element["columns"][LIST_OBJECT_DESC]["type"] = "text";
							element["columns"][LIST_OBJECT_DESC]["value"] = details->desc;
							element["columns"][LIST_OBJECT_OWNER]["column"] = "Owner";
							element["columns"][LIST_OBJECT_OWNER]["type"] = "text";
							element["columns"][LIST_OBJECT_OWNER]["value"] = object_owner;
							element["columns"][LIST_OBJECT_GROUP]["column"] = "Group";
							element["columns"][LIST_OBJECT_GROUP]["type"] = "text";
							element["columns"][LIST_OBJECT_GROUP]["value"] = object_group;
							mResultList->addElement(element, ADD_BOTTOM);
						}
					}
				}
			}
		}
	}

	mResultList->updateSort();
	mResultList->selectByID(selected);
	mResultList->setScrollPos(scrollpos);
	LLStringUtil::format_map_t args;
	args["[LISTED]"] = llformat("%d", mResultList->getItemCount());
	args["[PENDING]"] = llformat("%d", mRequested);
	args["[TOTAL]"] = llformat("%d", mObjectDetails.size());
	mCounterText->setText(getString("ListedPendingTotalFilled", args));
	mLastUpdateTimer.reset();
}


void FSAreaSearch::callbackLoadOwnerName(const LLUUID& id, const std::string& full_name)
{
	results();
}

void FSAreaSearch::processObjectPropertiesFamily(LLMessageSystem* msg)
{
	checkRegion();

	LLUUID object_id;
	msg->getUUIDFast(_PREHASH_ObjectData, _PREHASH_ObjectID, object_id);

	bool exists = (mObjectDetails.count(object_id) != 0);
	ObjectDetails *details = &mObjectDetails[object_id];
	if (!exists || details->name == request_string)
	{
		// We cache unknown objects (to avoid having to request them later)
		// and requested objects.
		if (exists && mRequested > 0) mRequested--;
		msg->getUUIDFast(_PREHASH_ObjectData, _PREHASH_OwnerID, details->owner_id);
		msg->getUUIDFast(_PREHASH_ObjectData, _PREHASH_GroupID, details->group_id);
		msg->getStringFast(_PREHASH_ObjectData, _PREHASH_Name, details->name);
		msg->getStringFast(_PREHASH_ObjectData, _PREHASH_Description, details->desc);
		gCacheName->get(details->owner_id, false, boost::bind(
							&FSAreaSearch::callbackLoadOwnerName, this, _1, _2));
		gCacheName->get(details->group_id, true, boost::bind(
							&FSAreaSearch::callbackLoadOwnerName, this, _1, _2));
	}
}

// static
bool FSAreaSearch::enableSelection(LLScrollListCtrl *ctrl, const LLSD &userdata)
{
	//
	//	WABBIT TODO: change the menu item's label to match the
	//	             object's touch name when one is defined
	//
 	LLScrollListItem *item = ctrl->getFirstSelected();
	if (!item) {
		return FALSE;
	}

	LLViewerObject *object = gObjectList.findObject(item->getUUID());
	if (!object) {
		return false;
	}

	std::string parameter = userdata.asString();

	if (parameter == "touch") {
		return object->flagHandleTouch();
	}
	else if (parameter == "return") {
		return enable_object_return();
	}
	else if (parameter == "take") {
		//
		//	WABBIT TODO: this doesn't work
		//
		return visible_take_object();
	}
	else if (parameter == "takecopy") {
		return enable_object_take_copy();
	}
	else if (parameter == "buy") {
		//
		//	WABBIT TODO: this doesn't work
		//
		return enable_buy_object();
	}
	else if (parameter == "pay") {
		return enable_pay_object();
	}
	else if (parameter == "delete") {
		return enable_object_delete();
	}

	return true;
}

// static
bool FSAreaSearch::handleSelection(LLScrollListCtrl *ctrl, const LLSD &userdata)
{
 	LLScrollListItem *item = ctrl->getFirstSelected();
	if (!item) {
		return false;
	}

	LLViewerObject *object = gObjectList.findObject(item->getValue().asUUID());
	if (!object) {
		return false;
	}

	std::string parameter = userdata.asString();

	if (parameter == "touch") {
		handle_object_touch();
	}
	else if (parameter == "teleport") {
		gAgent.teleportViaLocation(object->getPositionGlobal());
	}
	else if (parameter == "zoomin") {
		handle_zoom_to_object(item->getUUID());
	}
	else if (parameter == "track") {
		FSAreaSearch *floater = dynamic_cast<FSAreaSearch*>(gFloaterView->getParentFloater(ctrl));

		if (floater) {
			LLTracker::trackLocation(
				object->getPositionGlobal(),
				floater->getObjectName(item->getUUID()),
				"",
				LLTracker::LOCATION_ITEM
			);
		}
	}
	else if (parameter == "inspect") {
		handle_object_inspect();
	}
	else if (parameter == "report") {
		LLFloaterReporter::showFromObject(item->getValue().asUUID());
	}
	else if (parameter == "block") {
		FSAreaSearch *floater = dynamic_cast<FSAreaSearch*>(gFloaterView->getParentFloater(ctrl));
		LLMute mute(item->getValue().asUUID(), floater->getObjectName(item->getValue().asUUID()), LLMute::OBJECT);

		if (LLMuteList::getInstance()->isMuted(mute.mID)) {
			LLMuteList::getInstance()->remove(mute);
		}
		else {
			LLMuteList::getInstance()->add(mute);
			LLPanelBlockedList::showPanelAndSelect(mute.mID);
		}
	}
	else if (parameter == "return") {
		handle_object_return();
	}
	else if (parameter == "take") {
		handle_take();
	}
	else if (parameter == "takecopy") {
		handle_take_copy();
	}
	else if (parameter == "buy") {
		handle_buy();
	}
	else if (parameter == "pay") {
		handle_give_money_dialog();
	}
	else if (parameter == "delete") {
		handle_object_delete();
	}
	else {
		llwarns << "Unrecognised parameter (" << parameter << ")" << llendl;
		return false;
	}

	return true;
}

FSAreaSearch::ContextMenu::ContextMenu() :
	mParent(NULL)
{
}

void FSAreaSearch::ContextMenu::show(LLView *view, const uuid_vec_t &uuids, S32 x, S32 y)
{
	mParent = dynamic_cast<LLScrollListCtrl*>(view);
	LLListContextMenu::show(view, uuids, x, y);
	mParent = NULL;
}

LLContextMenu *FSAreaSearch::ContextMenu::createMenu()
{
	LLUICtrl::CommitCallbackRegistry::ScopedRegistrar commit_registrar;

	commit_registrar.add(
		"AreaSearch.Action",
		boost::bind(&FSAreaSearch::handleSelection, mParent, _2)
	);

	LLUICtrl::EnableCallbackRegistry::ScopedRegistrar enable_registrar;

	enable_registrar.add(
		"AreaSearch.Enable",
		boost::bind(&FSAreaSearch::enableSelection, mParent, _2)
	);

	return createFromFile("menu_area_search.xml");
}
