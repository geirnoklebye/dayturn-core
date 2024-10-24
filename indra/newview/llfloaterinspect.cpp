/** 
 * @file llfloaterinspect.cpp
 * @brief Floater for object inspection tool
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

#include "llfloaterinspect.h"

#include "llfloaterreg.h"
#include "llfloatertools.h"
#include "llavataractions.h"
#include "llavatarnamecache.h"
#include "llgroupactions.h"
#include "llscrolllistctrl.h"
#include "llscrolllistitem.h"
#include "llselectmgr.h"
#include "lltoolcomp.h"
#include "lltoolmgr.h"
#include "lltrans.h"
#include "llviewercontrol.h"
#include "llviewerobject.h"
#include "lluictrlfactory.h"
// [RLVa:KB] - Checked: RLVa-2.0.1
//#include "rlvactions.h"
//#include "rlvcommon.h"
//#include "rlvui.h"
// [/RLVa:KB]
// PoundLife - Improved Object Inspect
#include "llresmgr.h"
#include "lltexturectrl.h"
#include "llviewerobjectlist.h" //gObjectList
#include "llviewertexturelist.h"
// PoundLife END
#include "llvovolume.h"

// <FS:Ansariel> FIRE-22292: Configurable columns
#include "llmenugl.h"
#include "llmenubutton.h"
#include "lltoggleablemenu.h"
#include "llviewermenu.h"			// for gMenuHolder
// </FS:Ansariel>

//LLFloaterInspect* LLFloaterInspect::sInstance = NULL;

LLFloaterInspect::LLFloaterInspect(const LLSD& key)
  : LLFloater(key),
	mDirty(false),
	mOwnerNameCacheConnection(),
	mCreatorNameCacheConnection(),
	mPopupMenu(nullptr),
	// <FS:Ansariel> FIRE-22292: Configurable columns
	mOptionsButton(nullptr),
	mFSInspectColumnConfigConnection(),
	mLastResizeDelta(0)
	// </FS:Ansariel>
{
	mCommitCallbackRegistrar.add("Inspect.OwnerProfile",	boost::bind(&LLFloaterInspect::onClickOwnerProfile, this));
	mCommitCallbackRegistrar.add("Inspect.CreatorProfile",	boost::bind(&LLFloaterInspect::onClickCreatorProfile, this));
	mCommitCallbackRegistrar.add("Inspect.SelectObject",	boost::bind(&LLFloaterInspect::onSelectObject, this));

	// <FS:Ansariel> FIRE-22292: Configurable columns
	mColumnBits["object_name"] = 1;
	mColumnBits["description"] = 2;
	mColumnBits["owner_name"] = 4;
	mColumnBits["creator_name"] = 8;
	mColumnBits["facecount"] = 16;
	mColumnBits["vertexcount"] = 32;
	mColumnBits["trianglecount"] = 64;
	mColumnBits["tramcount"] = 128;
	mColumnBits["vramcount"] = 256;
	mColumnBits["creation_date"] = 512;
	// </FS:Ansariel>
}

bool LLFloaterInspect::postBuild()
{
	mObjectList = getChild<LLScrollListCtrl>("object_list");
//	childSetAction("button owner",onClickOwnerProfile, this);
//	childSetAction("button creator",onClickCreatorProfile, this);
//	childSetCommitCallback("object_list", onSelectObject, NULL);

	// <FS:Ansariel> FIRE-22292: Configurable columns
	LLUICtrl::CommitCallbackRegistry::ScopedRegistrar registrar;
	LLUICtrl::EnableCallbackRegistry::ScopedRegistrar enable_registrar;

	registrar.add("Inspect.ToggleColumn",			boost::bind(&LLFloaterInspect::onColumnVisibilityChecked, this, _2));
	enable_registrar.add("Inspect.EnableColumn",	boost::bind(&LLFloaterInspect::onEnableColumnVisibilityChecked, this, _2));

	mOptionsButton = getChild<LLMenuButton>("options_btn");

	LLToggleableMenu* options_menu  = LLUICtrlFactory::getInstance()->createFromFile<LLToggleableMenu>("menu_fs_inspect_options.xml", gMenuHolder, LLViewerMenuHolderGL::child_registry_t::instance());
	if (options_menu)
	{
		mOptionsMenuHandle = options_menu->getHandle();
		mOptionsButton->setMenu(options_menu, LLMenuButton::MP_BOTTOM_LEFT);
	}

	mFSInspectColumnConfigConnection = gSavedSettings.getControl("FSInspectColumnConfig")->getSignal()->connect(boost::bind(&LLFloaterInspect::onColumnDisplayModeChanged, this));
	onColumnDisplayModeChanged();
	// </FS:Ansariel>

	mObjectList->setRightMouseDownCallback(boost::bind(&LLFloaterInspect::onScrollListRightClicked, this, _1, _2, _3));
	
	refresh();
	
	return true;
}

LLFloaterInspect::~LLFloaterInspect(void)
{
	if (mOwnerNameCacheConnection.connected())
	{
		mOwnerNameCacheConnection.disconnect();
	}
	if (mCreatorNameCacheConnection.connected())
	{
		mCreatorNameCacheConnection.disconnect();
	}
	if(!LLFloaterReg::instanceVisible("build"))
	{
		if(LLToolMgr::getInstance()->getBaseTool() == LLToolCompInspect::getInstance())
		{
			LLToolMgr::getInstance()->clearTransientTool();
		}
		// Switch back to basic toolset
		LLToolMgr::getInstance()->setCurrentToolset(gBasicToolset);	
	}
	else
	{
		LLFloaterReg::showInstance("build", LLSD(), true);
	}

	// <FS:Ansariel> FIRE-22292: Configurable columns
	if (mFSInspectColumnConfigConnection.connected())
	{
		mFSInspectColumnConfigConnection.disconnect();
	}

	if (mOptionsMenuHandle.get()) mOptionsMenuHandle.get()->die();
	// </FS:Ansariel>
}

void LLFloaterInspect::onScrollListRightClicked(LLUICtrl* ctrl, S32 x, S32 y)
{
	LLScrollListItem *item = mObjectList->hitItem(x, y);
	if (item)
	{
		// check to see if we have a UUID for this row
		std::string id = item->getValue().asString();
		LLUUID uuid(id);
		if (! uuid.isNull())
		{
			mObjectList->selectByID(uuid);
			LLViewerObject* objectp = gObjectList.findObject(uuid);
			// if we arrived here with a child prim, switch to its root
			if (objectp != objectp->getRootEdit())
			{
				objectp = objectp->getRootEdit();
				uuid = objectp->getID();
			}
			// create the context menu from the XUI file and display it
			std::string menu_name = "menu_floater_inspect_list.xml";
			delete mPopupMenu;
			llassert(LLMenuGL::sMenuContainer != nullptr);
			mPopupMenu = LLUICtrlFactory::getInstance()->createFromFile<LLContextMenu>(
				menu_name, LLMenuGL::sMenuContainer, LLMenuHolderGL::child_registry_t::instance());
			mPopupMenu->show(x, y);
			((LLContextMenu*)mPopupMenu)->show(x, y);
			LLMenuGL::showPopup(ctrl, mPopupMenu, x, y);
		}
	}
}

void LLFloaterInspect::onOpen(const LLSD& key)
{
	bool forcesel = LLSelectMgr::getInstance()->setForceSelection(true);
	LLToolMgr::getInstance()->setTransientTool(LLToolCompInspect::getInstance());
	LLSelectMgr::getInstance()->setForceSelection(forcesel);	// restore previouis value
	mObjectSelection = LLSelectMgr::getInstance()->getSelection();
	refresh();
}

// [RLVa:KB] - Checked: RLVa-2.0.1
const LLSelectNode* LLFloaterInspect::getSelectedNode() /*const*/
{
	if(mObjectList->getAllSelected().size() == 0)
	{
		return nullptr;
	}
	LLScrollListItem* first_selected =mObjectList->getFirstSelected();

	if (first_selected)
	{
		struct f : public LLSelectedNodeFunctor
		{
			LLUUID obj_id;
			explicit f(const LLUUID& id) : obj_id(id) {}
			virtual bool apply(LLSelectNode* node)
			{
				return (obj_id == node->getObject()->getID());
			}
		} func(first_selected->getUUID());
		return mObjectSelection->getFirstNode(&func);
	}
	return nullptr;
}

void LLFloaterInspect::onClickCreatorProfile()
{
		const LLSelectNode* node = getSelectedNode();
		if(node)
		{
			// Only anonymize the creator if they're also the owner or if they're a nearby avie
			const LLUUID& idCreator = node->mPermissions->getCreator();
//			if ( (!RlvActions::canShowName(RlvActions::SNC_DEFAULT, idCreator)) && ((node->mPermissions->getOwner() == idCreator) || (RlvUtil::isNearbyAgent(idCreator))) )
//			{
//				return;
//			}
			LLAvatarActions::showProfile(idCreator);
		}
}

void LLFloaterInspect::onClickOwnerProfile()
{
		const LLSelectNode* node = getSelectedNode();
		if(node)
		{
			if(node->mPermissions->isGroupOwned())
			{
				const LLUUID& idGroup = node->mPermissions->getGroup();
				LLGroupActions::show(idGroup);
			}
			else
			{
				const LLUUID& owner_id = node->mPermissions->getOwner();
//				if (!RlvActions::canShowName(RlvActions::SNC_DEFAULT, owner_id))
//					return;
				LLAvatarActions::showProfile(owner_id);
			}

		}
}

void LLFloaterInspect::onSelectObject()
{
	if(LLFloaterInspect::getSelectedUUID() != LLUUID::null)
	{
//		if (!RlvActions::isRlvEnabled())
//		{
			getChildView("button owner")->setEnabled(true);
			getChildView("button creator")->setEnabled(true);
//		}
//		else
//		{
//			const LLSelectNode* node = getSelectedNode();
//			const LLUUID& idOwner = (node) ? node->mPermissions->getOwner() : LLUUID::null;
//			const LLUUID& idCreator = (node) ? node->mPermissions->getCreator() : LLUUID::null;
//
//			// See LLFloaterInspect::onClickCreatorProfile()
//			getChildView("button owner")->setEnabled( (RlvActions::canShowName(RlvActions::SNC_DEFAULT, idOwner)) || ((node) && (node->mPermissions->isGroupOwned())) );
//			// See LLFloaterInspect::onClickOwnerProfile()
//			getChildView("button creator")->setEnabled( ((idOwner != idCreator) && (!RlvUtil::isNearbyAgent(idCreator))) || (RlvActions::canShowName(RlvActions::SNC_DEFAULT, idCreator)) );
//		}
	}
}

LLUUID LLFloaterInspect::getSelectedUUID()
{
	if(mObjectList->getAllSelected().size() > 0)
	{
		LLScrollListItem* first_selected =mObjectList->getFirstSelected();
		if (first_selected)
		{
			return first_selected->getUUID();
		}
		
	}
	return LLUUID::null;
}

void LLFloaterInspect::refresh()
{
	LLUUID creator_id;
	std::string creator_name;
	S32 pos = mObjectList->getScrollPos();
	// PoundLife - Improved Object Inspect
	LLLocale locale("");
	LLResMgr& res_mgr = LLResMgr::instance();
	LLSelectMgr& sel_mgr = LLSelectMgr::instance();
	S32 fcount = 0;
	S32 tcount = 0;
	S32 vcount = 0;
	S32 objcount = 0;
	S32 primcount = 0;
	mTextureList.clear();
	mTextureMemory = 0;
	mTextureVRAMMemory = 0;
	// PoundLife - End
	getChildView("button owner")->setEnabled(false);
	getChildView("button creator")->setEnabled(false);
	LLUUID selected_uuid;
	S32 selected_index = mObjectList->getFirstSelectedIndex();
	if(selected_index > -1)
	{
		LLScrollListItem* first_selected =
			mObjectList->getFirstSelected();
		if (first_selected)
		{
			selected_uuid = first_selected->getUUID();
		}
	}
	mObjectList->operateOnAll(LLScrollListCtrl::OP_DELETE);
	//List all transient objects, then all linked objects

	for (LLObjectSelection::valid_iterator iter = mObjectSelection->valid_begin();
		 iter != mObjectSelection->valid_end(); iter++)
	{
		LLSelectNode* obj = *iter;
		LLSD row;
		std::string owner_name, creator_name;

		if (obj->mCreationDate == 0)
		{	// Don't have valid information from the server, so skip this one
			continue;
		}

		time_t timestamp = (time_t) (obj->mCreationDate/1000000);
		std::string timeStr = getString("timeStamp");
		LLSD substitution;
		substitution["datetime"] = (S32) timestamp;
		LLStringUtil::format (timeStr, substitution);

		const LLUUID& idOwner = obj->mPermissions->getOwner();
		const LLUUID& idCreator = obj->mPermissions->getCreator();
		LLAvatarName av_name;

		if(obj->mPermissions->isGroupOwned())
		{
			std::string group_name;
			const LLUUID& idGroup = obj->mPermissions->getGroup();
			if(gCacheName->getGroupName(idGroup, group_name))
			{
				// <FS:Ansariel> Make text localizable
				//owner_name = "[" + group_name + "] (group)";
				owner_name = "[" + group_name + "] " + getString("Group");
			}
			else
			{
				owner_name = LLTrans::getString("RetrievingData");
				if (mOwnerNameCacheConnection.connected())
				{
					mOwnerNameCacheConnection.disconnect();
				}
				mOwnerNameCacheConnection = gCacheName->getGroup(idGroup, boost::bind(&LLFloaterInspect::onGetOwnerNameCallback, this));
			}
		}
		else
		{
			// Only work with the names if we actually get a result
			// from the name cache. If not, defer setting the
			// actual name and set a placeholder.
			if (LLAvatarNameCache::get(idOwner, &av_name))
			{
// [RLVa:KB] - Checked: RLVa-2.0.1
//				bool fRlvCanShowName = (RlvActions::canShowName(RlvActions::SNC_DEFAULT, idOwner)) || (obj->mPermissions->isGroupOwned());
//				owner_name = (fRlvCanShowName) ? av_name.getCompleteName() : RlvStrings::getAnonym(av_name);
// [/RLVa:KB]
				owner_name = av_name.getCompleteName();
			}
			else
			{
				owner_name = LLTrans::getString("RetrievingData");
				if (mOwnerNameCacheConnection.connected())
				{
					mOwnerNameCacheConnection.disconnect();
				}
				mOwnerNameCacheConnection = LLAvatarNameCache::get(idOwner, boost::bind(&LLFloaterInspect::onGetOwnerNameCallback, this));
			}
		}

		if (LLAvatarNameCache::get(idCreator, &av_name))
		{
// [RLVa:KB] - Checked: RLVa-2.0.1
//			const LLUUID& idCreator = obj->mPermissions->getCreator();
//			bool fRlvCanShowName = (RlvActions::canShowName(RlvActions::SNC_DEFAULT, idCreator)) || ( (obj->mPermissions->getOwner() != idCreator) && (!RlvUtil::isNearbyAgent(idCreator)) );
//			creator_name = (fRlvCanShowName) ? av_name.getCompleteName() : RlvStrings::getAnonym(av_name);
// [/RLVa:KB]
			creator_name = av_name.getCompleteName();
		}
		else
		{
			creator_name = LLTrans::getString("RetrievingData");
			if (mCreatorNameCacheConnection.connected())
			{
				mCreatorNameCacheConnection.disconnect();
			}
			mCreatorNameCacheConnection = LLAvatarNameCache::get(idCreator, boost::bind(&LLFloaterInspect::onGetCreatorNameCallback, this));
		}
		
		row["id"] = obj->getObject()->getID();
		row["columns"][0]["column"] = "object_name";
		row["columns"][0]["type"] = "text";
		// make sure we're either at the top of the link chain
		// or top of the editable chain, for attachments
		if(!(obj->getObject()->isRoot() || obj->getObject()->isRootEdit()))
		{
			row["columns"][0]["value"] = std::string("   ") + obj->mName;
		}
		else
		{
			row["columns"][0]["value"] = obj->mName;
		}
		row["columns"][1]["column"] = "owner_name";
		row["columns"][1]["type"] = "text";
		row["columns"][1]["value"] = owner_name;
		row["columns"][2]["column"] = "creator_name";
		row["columns"][2]["type"] = "text";
		row["columns"][2]["value"] = creator_name;
		row["columns"][3]["column"] = "creation_date";
		row["columns"][3]["type"] = "text";
		row["columns"][3]["value"] = timeStr;
		// <FS:PP> FIRE-12854: Include a Description column in the Inspect Objects floater
		row["columns"][4]["column"] = "description";
		row["columns"][4]["type"] = "text";
		row["columns"][4]["value"] = obj->mDescription;
		// </FS:PP>
		// <FS:Ansariel> Correct creation date sorting
		row["columns"][5]["column"] = "creation_date_sort";
		row["columns"][5]["type"] = "text";
		row["columns"][5]["value"] = llformat("%d", timestamp);
		// </FS:Ansariel>

		// PoundLife - Improved Object Inspect
		
	    std::string fcount_string;
		res_mgr.getIntegerString(fcount_string, obj->getObject()->getNumFaces());
		row["columns"][4]["column"] = "facecount";
		row["columns"][4]["type"] = "text";
		row["columns"][4]["value"] = fcount_string;

	    std::string vcount_string;
		res_mgr.getIntegerString(vcount_string, obj->getObject()->getNumVertices());
		row["columns"][5]["column"] = "vertexcount";
		row["columns"][5]["type"] = "text";
		row["columns"][5]["value"] = vcount_string;

	    std::string tcount_string;
		res_mgr.getIntegerString(tcount_string, obj->getObject()->getNumIndices() / 3);
		row["columns"][6]["column"] = "trianglecount";
		row["columns"][6]["type"] = "text";
		row["columns"][6]["value"] = tcount_string;

		// Poundlife - Get VRAM
		U32 texture_memory = 0;
		U32 vram_memory = 0;
		getObjectTextureMemory(obj->getObject(), texture_memory, vram_memory);
		primcount = sel_mgr.getSelection()->getObjectCount();
		objcount = sel_mgr.getSelection()->getRootObjectCount();
		fcount += obj->getObject()->getNumFaces();
		tcount += obj->getObject()->getNumIndices() / 3;
		vcount += obj->getObject()->getNumVertices();
		// PoundLife - END
		mObjectList->addElement(row, ADD_TOP);
	}

	if(selected_index > -1 && mObjectList->getItemIndex(selected_uuid) == selected_index)
	{
		mObjectList->selectNthItem(selected_index);
	}
	else
	{
		mObjectList->selectNthItem(0);
	}
	onSelectObject();
	mObjectList->setScrollPos(pos);


	// PoundLife - Total linkset stats.
	LLStringUtil::format_map_t args;
	std::string objcount_string;
	res_mgr.getIntegerString(objcount_string, objcount);
	args["NUM_OBJECTS"] = objcount_string;
	std::string primcount_string;
	res_mgr.getIntegerString(primcount_string, primcount);
	args["NUM_PRIMS"] = primcount_string;
	std::string ftotcount_string;
	res_mgr.getIntegerString(ftotcount_string, fcount);
	args["NUM_FACES"] = ftotcount_string;
	std::string vtotcount_string;
	res_mgr.getIntegerString(vtotcount_string, vcount);
	args["NUM_VERTICES"] = vtotcount_string;
	std::string ttotcount_string;
	res_mgr.getIntegerString(ttotcount_string, tcount);
	args["NUM_TRIANGLES"] = ttotcount_string;
	std::string texcount_string;
	res_mgr.getIntegerString(texcount_string, mTextureList.size());
	args["NUM_TEXTURES"] = texcount_string;
	std::string texmem_string;
	res_mgr.getIntegerString(texmem_string, mTextureMemory / 1024);
	args["TEXTURE_MEMORY"] = texmem_string;
	std::string vram_string;
	res_mgr.getIntegerString(vram_string, mTextureVRAMMemory / 1024);
	args["VRAM_USAGE"] = vram_string;
	getChild<LLTextBase>("linksetstats_text")->setText(getString("stats_list", args));
	// PoundLife - End
}

// PoundLife - Improved Object Inspect
void LLFloaterInspect::getObjectTextureMemory(LLViewerObject* object, U32& object_texture_memory, U32& object_vram_memory)
{
	uuid_vec_t object_texture_list;

	if (!object)
	{
		return;
	}

	LLUUID uuid;
	U8 te_count = object->getNumTEs();

	for (U8 j = 0; j < te_count; j++)
	{
		LLViewerTexture* img = object->getTEImage(j);
		if (img)
		{
			calculateTextureMemory(img, object_texture_list, object_texture_memory, object_vram_memory);
		}

		// materials per face
		if (object->getTE(j)->getMaterialParams().notNull())
		{
			uuid = object->getTE(j)->getMaterialParams()->getNormalID();
			if (uuid.notNull())
			{
				LLViewerTexture* img = gTextureList.getImage(uuid);
				if (img)
				{
					calculateTextureMemory(img, object_texture_list, object_texture_memory, object_vram_memory);
				}
			}

			uuid = object->getTE(j)->getMaterialParams()->getSpecularID();
			if (uuid.notNull())
			{
				LLViewerTexture* img = gTextureList.getImage(uuid);
				if (img)
				{
					calculateTextureMemory(img, object_texture_list, object_texture_memory, object_vram_memory);
				}
			}
		}
	}

	// sculpt map
	if (object->isSculpted() && !object->isMesh())
	{
		LLSculptParams *sculpt_params = (LLSculptParams *)(object->getParameterEntry(LLNetworkData::PARAMS_SCULPT));
		uuid = sculpt_params->getSculptTexture();
		LLViewerTexture* img = gTextureList.getImage(uuid);
		if (img)
		{
			calculateTextureMemory(img, object_texture_list, object_texture_memory, object_vram_memory);
		}
	}
}

void LLFloaterInspect::calculateTextureMemory(LLViewerTexture* texture, uuid_vec_t& object_texture_list, U32& object_texture_memory, U32& object_vram_memory)
{
	const LLUUID uuid = texture->getID();
	U32 vram_memory = (texture->getFullHeight() * texture->getFullWidth() * 32 / 8);
	U32 texture_memory = (texture->getFullHeight() * texture->getFullWidth() * texture->getComponents());

	if (std::find(mTextureList.begin(), mTextureList.end(), uuid) == mTextureList.end())
	{
		mTextureList.push_back(uuid);
		mTextureMemory += texture_memory;
		mTextureVRAMMemory += vram_memory;
	}

	if (std::find(object_texture_list.begin(), object_texture_list.end(), uuid) == object_texture_list.end())
	{
		object_texture_list.push_back(uuid);
		object_texture_memory += texture_memory;
		object_vram_memory += vram_memory;
	}
}
// PoundLife - End

void LLFloaterInspect::onFocusReceived()
{
	LLToolMgr::getInstance()->setTransientTool(LLToolCompInspect::getInstance());
	LLFloater::onFocusReceived();
}

void LLFloaterInspect::dirty()
{
	setDirty();
}

void LLFloaterInspect::onGetOwnerNameCallback()
{
	mOwnerNameCacheConnection.disconnect();
	setDirty();
}

void LLFloaterInspect::onGetCreatorNameCallback()
{
	mCreatorNameCacheConnection.disconnect();
	setDirty();
}

void LLFloaterInspect::draw()
{
	if (mDirty)
	{
		refresh();
		mDirty = false;
	}

	LLFloater::draw();
}

// <FS:Ansariel> FIRE-22292: Configurable columns
void LLFloaterInspect::onColumnDisplayModeChanged()
{
	U32 column_config = gSavedSettings.getU32("FSInspectColumnConfig");
	std::vector<LLScrollListColumn::Params> column_params = mObjectList->getColumnInitParams();
	S32 column_padding = mObjectList->getColumnPadding();

	S32 default_width = 0;
	S32 new_width = 0;
	S32 min_width, min_height;
	getResizeLimits(&min_width, &min_height);

	std::string current_sort_col = mObjectList->getSortColumnName();
	bool current_sort_asc = mObjectList->getSortAscending();
	
	mObjectList->clearRows();
	mObjectList->clearColumns();
	mObjectList->updateLayout();

	std::vector<LLScrollListColumn::Params>::iterator param_it;
	for (param_it = column_params.begin(); param_it != column_params.end(); ++param_it)
	{
		LLScrollListColumn::Params p = *param_it;
		default_width += (p.width.pixel_width.getValue() + column_padding);
		
		LLScrollListColumn::Params params;
		params.header = p.header;
		params.name = p.name;
		params.halign = p.halign;
		params.sort_direction = p.sort_direction;
		params.sort_column = p.sort_column;
		params.tool_tip = p.tool_tip;

		if (column_config & mColumnBits[p.name.getValue()])
		{
			params.width = p.width;
			new_width += (params.width.pixel_width.getValue() + column_padding);
		}
		else
		{
			params.width.pixel_width.set(-1, true);
		}

		mObjectList->addColumn(params);
	}

	min_width -= (default_width - new_width - mLastResizeDelta);
	mLastResizeDelta = default_width - new_width;
	setResizeLimits(min_width, min_height);

	if (getRect().getWidth() < min_width)
	{
		reshape(min_width, getRect().getHeight());
	}

	if (!current_sort_col.empty())
	{
		if ((current_sort_col == "creation_date_sort" && mObjectList->getColumn("creation_date")->getWidth() == -1) ||
			mObjectList->getColumn(current_sort_col)->getWidth() == -1)
		{
			mObjectList->clearSortOrder();
		}
		else
		{
			mObjectList->sortByColumn(current_sort_col, current_sort_asc);
		}
	}
	mObjectList->setFilterColumn(0);
	mObjectList->dirtyColumns();
	setDirty();
}

void LLFloaterInspect::onColumnVisibilityChecked(const LLSD& userdata)
{
	std::string column = userdata.asString();
	U32 column_config = gSavedSettings.getU32("FSInspectColumnConfig");

	U32 new_value;
	U32 enabled = (mColumnBits[column] & column_config);
	if (enabled)
	{
		new_value = (column_config & ~mColumnBits[column]);
	}
	else
	{
		new_value = (column_config | mColumnBits[column]);
	}

	gSavedSettings.setU32("FSInspectColumnConfig", new_value);
}

bool LLFloaterInspect::onEnableColumnVisibilityChecked(const LLSD& userdata)
{
	std::string column = userdata.asString();
	U32 column_config = gSavedSettings.getU32("FSInspectColumnConfig");

	return (mColumnBits[column] & column_config);
}
// </FS:Ansariel>
