/** 
* @file llfloatersimplesnapshot.cpp
* @brief Snapshot preview window for saving as a thumbnail
*
* $LicenseInfo:firstyear=2022&license=viewerlgpl$
* Second Life Viewer Source Code
* Copyright (C) 2022, Linden Research, Inc.
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

#include "llfloatersimplesnapshot.h"

#include "llfloaterreg.h"
#include "llimagefiltersmanager.h"
#include "llstatusbar.h" // can_afford_transaction()
#include "llnotificationsutil.h"
#include "lloutfitgallery.h"
#include "llagentbenefits.h"
#include "llviewercontrol.h"

LLSimpleSnapshotFloaterView* gSimpleSnapshotFloaterView = NULL;

const S32 THUMBNAIL_SNAPSHOT_WIDTH = 256;
const S32 THUMBNAIL_SNAPSHOT_HEIGHT = 256;

static LLDefaultChildRegistry::Register<LLSimpleSnapshotFloaterView> r("simple_snapshot_floater_view");

///----------------------------------------------------------------------------
/// Class LLFloaterSimpleSnapshot::Impl
///----------------------------------------------------------------------------

LLSnapshotModel::ESnapshotFormat LLFloaterSimpleSnapshot::Impl::getImageFormat(LLFloaterSnapshotBase* floater)
{
    return LLSnapshotModel::SNAPSHOT_FORMAT_PNG;
}

LLSnapshotModel::ESnapshotLayerType LLFloaterSimpleSnapshot::Impl::getLayerType(LLFloaterSnapshotBase* floater)
{
    return LLSnapshotModel::SNAPSHOT_TYPE_COLOR;
}

void LLFloaterSimpleSnapshot::Impl::updateControls(LLFloaterSnapshotBase* floater)
{
    LLSnapshotLivePreview* previewp = getPreviewView();
    updateResolution(floater);
    if (previewp)
    {
        previewp->setSnapshotType(LLSnapshotModel::ESnapshotType::SNAPSHOT_TEXTURE);
        previewp->setSnapshotFormat(LLSnapshotModel::ESnapshotFormat::SNAPSHOT_FORMAT_PNG);
        previewp->setSnapshotBufferType(LLSnapshotModel::ESnapshotLayerType::SNAPSHOT_TYPE_COLOR);
    }
}

std::string LLFloaterSimpleSnapshot::Impl::getSnapshotPanelPrefix()
{
    return "panel_outfit_snapshot_";
}

void LLFloaterSimpleSnapshot::Impl::updateResolution(void* data)
{
    LLFloaterSimpleSnapshot *view = (LLFloaterSimpleSnapshot *)data;

    if (!view)
    {
        llassert(view);
        return;
    }

    S32 width = THUMBNAIL_SNAPSHOT_WIDTH;
    S32 height = THUMBNAIL_SNAPSHOT_HEIGHT;

    LLSnapshotLivePreview* previewp = getPreviewView();
    if (previewp)
    {
        S32 original_width = 0, original_height = 0;
        previewp->getSize(original_width, original_height);

        if (gSavedSettings.getbool("RenderHUDInSnapshot"))
        { //clamp snapshot resolution to window size when showing UI HUD in snapshot
            width = llmin(width, gViewerWindow->getWindowWidthRaw());
            height = llmin(height, gViewerWindow->getWindowHeightRaw());
        }

        llassert(width > 0 && height > 0);

        previewp->setSize(width, height);

        if (original_width != width || original_height != height)
        {
            // hide old preview as the aspect ratio could be wrong
            checkAutoSnapshot(previewp, false);
            previewp->updateSnapshot(true);
        }
    }
}

void LLFloaterSimpleSnapshot::Impl::setStatus(EStatus status, bool ok, const std::string& msg)
{
    switch (status)
    {
    case STATUS_READY:
        mFloater->setCtrlsEnabled(true);
        break;
    case STATUS_WORKING:
        mFloater->setCtrlsEnabled(false);
        break;
    case STATUS_FINISHED:
        mFloater->setCtrlsEnabled(true);
        break;
    }

    mStatus = status;
}

///----------------------------------------------------------------re------------
/// Class LLFloaterSimpleSnapshot
///----------------------------------------------------------------------------

LLFloaterSimpleSnapshot::LLFloaterSimpleSnapshot(const LLSD& key)
    : LLFloaterSnapshotBase(key),
    mOutfitGallery(NULL)
{
    impl = new Impl(this);
}

LLFloaterSimpleSnapshot::~LLFloaterSimpleSnapshot()
{
}

bool LLFloaterSimpleSnapshot::postBuild()
{
    getChild<LLUICtrl>("save_btn")->setLabelArg("[UPLOAD_COST]", std::to_string(LLAgentBenefitsMgr::current().getTextureUploadCost()));

    childSetAction("new_snapshot_btn", ImplBase::onClickNewSnapshot, this);
    childSetAction("save_btn", boost::bind(&LLFloaterSimpleSnapshot::onSend, this));
    childSetAction("cancel_btn", boost::bind(&LLFloaterSimpleSnapshot::onCancel, this));

    mThumbnailPlaceholder = getChild<LLUICtrl>("thumbnail_placeholder");

    // create preview window
    LLRect full_screen_rect = getRootView()->getRect();
    LLSnapshotLivePreview::Params p;
    p.rect(full_screen_rect);
    LLSnapshotLivePreview* previewp = new LLSnapshotLivePreview(p);
    LLView* parent_view = gSnapshotFloaterView->getParent();

    parent_view->removeChild(gSnapshotFloaterView);
    // make sure preview is below snapshot floater
    parent_view->addChild(previewp);
    parent_view->addChild(gSnapshotFloaterView);

    //move snapshot floater to special purpose snapshotfloaterview
    gFloaterView->removeChild(this);
    gSnapshotFloaterView->addChild(this);

    impl->mPreviewHandle = previewp->getHandle();
    previewp->setContainer(this);
    impl->updateControls(this);
    impl->setAdvanced(true);
    impl->setSkipReshaping(true);

    previewp->mKeepAspectRatio = false;
    previewp->setThumbnailPlaceholderRect(getThumbnailPlaceholderRect());
    previewp->setAllowRenderUI(false);

    return true;
}
const S32 PREVIEW_OFFSET_X = 12;
const S32 PREVIEW_OFFSET_Y = 70;

void LLFloaterSimpleSnapshot::draw()
{
    LLSnapshotLivePreview* previewp = getPreviewView();

    if (previewp && (previewp->isSnapshotActive() || previewp->getThumbnailLock()))
    {
        // don't render snapshot window in snapshot, even if "show ui" is turned on
        return;
    }

    LLFloater::draw();

    if (previewp && !isMinimized() && mThumbnailPlaceholder->getVisible())
    {		
        if(previewp->getThumbnailImage())
        {
            bool working = impl->getStatus() == ImplBase::STATUS_WORKING;
            const LLRect& thumbnail_rect = getThumbnailPlaceholderRect();
            const S32 thumbnail_w = previewp->getThumbnailWidth();
            const S32 thumbnail_h = previewp->getThumbnailHeight();

            S32 offset_x = PREVIEW_OFFSET_X;
            S32 offset_y = PREVIEW_OFFSET_Y;

            gGL.matrixMode(LLRender::MM_MODELVIEW);
            // Apply floater transparency to the texture unless the floater is focused.
            F32 alpha = getTransparencyType() == TT_ACTIVE ? 1.0f : getCurrentTransparency();
            LLColor4 color = working ? LLColor4::grey4 : LLColor4::white;
            gl_draw_scaled_image(offset_x, offset_y, 
                thumbnail_w, thumbnail_h,
                previewp->getThumbnailImage(), color % alpha);
#if LL_DARWIN
            std::string alpha_color = getTransparencyType() == TT_ACTIVE ? "OutfitSnapshotMacMask" : "OutfitSnapshotMacMask2";
#else
            std::string alpha_color = getTransparencyType() == TT_ACTIVE ? "FloaterFocusBackgroundColor" : "DkGray";
#endif

            previewp->drawPreviewRect(offset_x, offset_y, LLUIColorTable::instance().getColor(alpha_color));

            gGL.pushUIMatrix();
            LLUI::translate((F32) thumbnail_rect.mLeft, (F32) thumbnail_rect.mBottom);
            mThumbnailPlaceholder->draw();
            gGL.popUIMatrix();
        }
    }
    impl->updateLayout(this);
}

void LLFloaterSimpleSnapshot::onOpen(const LLSD& key)
{
    LLSnapshotLivePreview* preview = getPreviewView();
    if (preview)
    {
        preview->updateSnapshot(true);
    }
    focusFirstItem(false);
    gSnapshotFloaterView->setEnabled(true);
    gSnapshotFloaterView->setVisible(true);
    gSnapshotFloaterView->adjustToFitScreen(this, false);

    impl->updateControls(this);
    impl->setStatus(ImplBase::STATUS_READY);
}

void LLFloaterSimpleSnapshot::onCancel()
{
    closeFloater();
}

void LLFloaterSimpleSnapshot::onSend()
{
    S32 expected_upload_cost = LLAgentBenefitsMgr::current().getTextureUploadCost();
    if (can_afford_transaction(expected_upload_cost))
    {
        saveTexture();
        postSave();
    }
    else
    {
        LLSD args;
        args["COST"] = llformat("%d", expected_upload_cost);
        LLNotificationsUtil::add("ErrorPhotoCannotAfford", args);
        inventorySaveFailed();
    }
}

void LLFloaterSimpleSnapshot::postSave()
{
    impl->setStatus(ImplBase::STATUS_WORKING);
}

// static 
void LLFloaterSimpleSnapshot::update()
{
    LLFloaterSimpleSnapshot* inst = findInstance();
    if (inst != NULL)
    {
        inst->impl->updateLivePreview();
    }
}


// static
LLFloaterSimpleSnapshot* LLFloaterSimpleSnapshot::findInstance()
{
    return LLFloaterReg::findTypedInstance<LLFloaterSimpleSnapshot>("simple_outfit_snapshot");
}

// static
LLFloaterSimpleSnapshot* LLFloaterSimpleSnapshot::getInstance()
{
    return LLFloaterReg::getTypedInstance<LLFloaterSimpleSnapshot>("simple_outfit_snapshot");
}

void LLFloaterSimpleSnapshot::saveTexture()
{
     LLSnapshotLivePreview* previewp = getPreviewView();
    if (!previewp)
    {
        llassert(previewp != NULL);
        return;
    }

    if (mOutfitGallery)
    {
        mOutfitGallery->onBeforeOutfitSnapshotSave();
    }
    previewp->saveTexture(true, getOutfitID().asString());
    if (mOutfitGallery)
    {
        mOutfitGallery->onAfterOutfitSnapshotSave();
    }
    closeFloater();
}

///----------------------------------------------------------------------------
/// Class LLSimpleOutfitSnapshotFloaterView
///----------------------------------------------------------------------------

LLSimpleSnapshotFloaterView::LLSimpleSnapshotFloaterView(const Params& p) : LLFloaterView(p)
{
}

LLSimpleSnapshotFloaterView::~LLSimpleSnapshotFloaterView()
{
}
