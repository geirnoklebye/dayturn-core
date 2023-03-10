/** 
 * @file llpanelpeoplemenus.h
 * @brief Menus used by the side tray "People" panel
 *
 * $LicenseInfo:firstyear=2009&license=viewerlgpl$
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

#ifndef LL_LLPANELPEOPLEMENUS_H
#define LL_LLPANELPEOPLEMENUS_H

#include "lllistcontextmenu.h"

namespace LLPanelPeopleMenus
{

/**
 * Menu used in the people lists.
 */
class PeopleContextMenu : public LLListContextMenu
{
public:
	/*virtual*/ LLContextMenu* createMenu();

protected:
	virtual void buildContextMenu(class LLMenuGL& menu, U32 flags);

private:
	bool enableContextMenuItem(const LLSD& userdata);
	bool checkContextMenuItem(const LLSD& userdata);
	bool enableFreezeEject(const LLSD& userdata);
	void offerTeleport();
	void eject();
	void startConference();
	void requestTeleport();
	// <FS:Ansariel> Add to contact set
	void addToContactSet();
	void toggleRights(const LLSD& userdata);

	void handle_avatar_grant_online_status(const LLUUID& avatar_id);
	void handle_avatar_grant_map_location(const LLUUID& avatar_id);
	void handle_avatar_grant_modify_objects(const LLUUID& avatar_id);

	void toggle_rights(const LLUUID& avatar_id, S32 rights);
	void confirm_modify_rights(const LLUUID& avatar_id, const bool grant, const S32 rights);
	void rights_confirmation_callback(const LLSD& notification, const LLSD& response, const LLUUID& avatar_id, const S32 rights);
	void handleMark(const LLSD& userdata);
	void handleClearMark();
	void handleClearMarks();
};

/**
 * Menu used in the nearby people list.
 */
class NearbyPeopleContextMenu : public PeopleContextMenu
{
protected:
	/*virtual*/ void buildContextMenu(class LLMenuGL& menu, U32 flags);
};

extern PeopleContextMenu gPeopleContextMenu;
extern NearbyPeopleContextMenu gNearbyPeopleContextMenu;

} // namespace LLPanelPeopleMenus

#endif // LL_LLPANELPEOPLEMENUS_H
