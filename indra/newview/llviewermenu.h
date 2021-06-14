/** 
 * @file llviewermenu.h
 * @brief Builds menus out of objects
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

#ifndef LL_LLVIEWERMENU_H
#define LL_LLVIEWERMENU_H

#include "../llui/llmenugl.h"
#include "llsafehandle.h"

class LLMessageSystem;
class LLSD;
class LLUICtrl;
class LLView;
class LLParcelSelection;
class LLObjectSelection;
class LLSelectNode;
class PieMenu;			// ## Zi: Pie menu

void initialize_edit_menu();
void initialize_spellcheck_menu();
void init_menus();
void cleanup_menus();

void show_debug_menus(); // checks for if menus should be shown first.
void kokua_menus();
void toggle_debug_menus(void*);
void show_context_menu( S32 x, S32 y, MASK mask );
void show_build_mode_context_menu(S32 x, S32 y, MASK mask);
void show_navbar_context_menu(LLView* ctrl, S32 x, S32 y);
void show_topinfobar_context_menu(LLView* ctrl, S32 x, S32 y);
void handle_reset_view();
void handle_cut(void*);
void handle_copy(void*);
void handle_paste(void*);
void handle_delete(void*);
void handle_redo(void*);
void handle_undo(void*);
void handle_select_all(void*);
void handle_deselect(void*);
void handle_delete_object();
void handle_duplicate(void*);
void handle_duplicate_in_place(void*);
void handle_object_scripts(std::string action);
BOOL enable_not_have_card(void *userdata);
void process_grant_godlike_powers(LLMessageSystem* msg, void**);

BOOL enable_cut(void*);
BOOL enable_copy(void*);
BOOL enable_paste(void*);
BOOL enable_select_all(void*);
BOOL enable_deselect(void*);
BOOL enable_undo(void*);
BOOL enable_redo(void*);

BOOL is_agent_mappable(const LLUUID& agent_id);

void confirm_replace_attachment(S32 option, void* user_data);
void handle_detach_from_avatar(const LLSD& user_data);
void attach_label(std::string& label, const LLSD&);
void detach_label(std::string& label, const LLSD&);
void handle_detach(void*);
// [SL:KB] - Patch: Inventory-AttachmentActions - Checked: 2012-05-05 (Catznip-3.3)
void handle_attachment_edit(const LLUUID& idItem);
void handle_item_edit(const LLUUID& idItem);
bool enable_item_edit(const LLUUID& idItem);
void handle_attachment_touch(const LLUUID& idItem);
bool enable_attachment_touch(const LLUUID& idItem);
// [/SL:KB]
BOOL enable_god_full(void* user_data);
BOOL enable_god_liaison(void* user_data);
BOOL enable_god_basic(void* user_data);
void check_merchant_status(bool force = false);

void exchange_callingcard(const LLUUID& dest_id);

void handle_gestures(void*);
void handle_sit_down(void*);
void handle_object_build(void*);
void handle_object_edit();
void handle_object_touch();
bool enable_object_open();
void handle_object_open();
void handle_object_scripts(std::string action);
void handle_object_derender(const LLSD& sdParam);

bool visible_take_object();
bool tools_visible_take_object();
bool enable_object_take_copy();
bool enable_object_return();
bool enable_object_delete();

// Buy either contents or object itself
void handle_buy();
void handle_take();
void handle_take_copy();
void handle_look_at_selection(const LLSD& param);
void handle_zoom_to_object(LLUUID object_id);
void handle_object_return();
void handle_object_delete();
void handle_object_edit();

void handle_attachment_edit(const LLUUID& inv_item_id);
void handle_attachment_touch(const LLUUID& inv_item_id);
bool enable_attachment_touch(const LLUUID& inv_item_id);

void handle_buy_land();

// Takes avatar UUID, or if no UUID passed, uses last selected object
void handle_avatar_freeze(const LLSD& avatar_id);

// Takes avatar UUID, or if no UUID passed, uses last selected object
void handle_avatar_eject(const LLSD& avatar_id);

bool enable_freeze_eject(const LLSD& avatar_id);

// Can anyone take a free copy of the object?
// *TODO: Move to separate file
bool anyone_copy_selection(LLSelectNode* nodep);

// Is this selected object for sale?
// *TODO: Move to separate file
bool for_sale_selection(LLSelectNode* nodep);

void handle_save_snapshot(void *);
void handle_toggle_flycam();

void handle_object_sit_or_stand();
void handle_object_teleport();
void handle_object_inspect();
void handle_give_money_dialog();
bool enable_pay_object();
bool enable_buy_object();
bool handle_go_to();

// Export to XML or Collada
void handle_export_selected( void * );

// Convert strings to internal types
U32 render_type_from_string(std::string render_type);
U32 feature_from_string(std::string feature);
U64 info_display_from_string(std::string info_display);
// <FS:Techwolf Lupindo> export
bool enable_object_export();
// </FS:Techwolf Lupindo>


class LLViewerMenuHolderGL : public LLMenuHolderGL
{
public:
	struct Params : public LLInitParam::Block<Params, LLMenuHolderGL::Params>
	{};

	LLViewerMenuHolderGL(const Params& p);

	virtual BOOL hideMenus();
	
	void setParcelSelection(LLSafeHandle<LLParcelSelection> selection);
	void setObjectSelection(LLSafeHandle<LLObjectSelection> selection);

	virtual const LLRect getMenuRect() const;

protected:
	LLSafeHandle<LLParcelSelection> mParcelSelection;
	LLSafeHandle<LLObjectSelection> mObjectSelection;
};

extern LLMenuBarGL*		gMenuBarView;
//extern LLView*			gMenuBarHolder;
extern LLMenuGL*		gEditMenu;
extern LLMenuGL*		gPopupMenuView;
extern LLViewerMenuHolderGL*	gMenuHolder;
extern LLMenuBarGL*		gLoginMenuBarView;

// Context menus in 3D scene
extern LLContextMenu		*gMenuAvatarSelf;
extern LLContextMenu		*gMenuAvatarOther;
extern LLContextMenu		*gMenuObject;
extern LLContextMenu		*gMenuAttachmentSelf;
extern LLContextMenu		*gMenuAttachmentOther;
extern LLContextMenu		*gMenuLand;
extern LLContextMenu		*gMenuMuteParticle;

// Needed to build menus when attachment site list available
extern LLMenuGL* gAttachSubMenu;
extern LLMenuGL* gDetachSubMenu;
extern LLMenuGL* gTakeOffClothes;
extern LLMenuGL* gDetachAvatarMenu;
extern LLMenuGL* gDetachHUDAvatarMenu;
extern LLContextMenu* gAttachScreenPieMenu;
extern LLContextMenu* gDetachScreenPieMenu;
extern LLContextMenu* gDetachHUDAttSelfMenu;
extern LLContextMenu* gAttachPieMenu;
extern LLContextMenu* gDetachPieMenu;
extern LLContextMenu* gDetachAttSelfMenu;
extern LLContextMenu* gAttachBodyPartPieMenus[9];
extern LLContextMenu* gDetachBodyPartPieMenus[9];

// ## Zi: Pie menus in 3D scene
extern PieMenu			*gPieMenuAvatarSelf;
extern PieMenu			*gPieMenuAvatarOther;
extern PieMenu			*gPieMenuObject;
extern PieMenu			*gPieMenuAttachmentSelf;
extern PieMenu			*gPieMenuAttachmentOther;
extern PieMenu			*gPieMenuLand;
// Needed to build pie menus when attachment site list available
extern PieMenu* gPieAttachScreenMenu;
extern PieMenu* gPieDetachScreenMenu;
extern PieMenu* gPieAttachMenu;
extern PieMenu* gPieDetachMenu;
extern PieMenu* gPieAttachBodyPartMenus[8];
extern PieMenu* gPieDetachBodyPartMenus[8];


/*
// Dead code?
extern LLMenuItemCallGL* gMutePieMenu;
extern LLMenuItemCallGL* gMuteObjectPieMenu;
extern LLMenuItemCallGL* gBuyPassPieMenu;
// Dead code?
*/

#endif
