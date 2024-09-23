/** 
 * @file llpreviewanim.cpp
 * @brief LLPreviewAnim class implementation
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

#include "llpreviewanim.h"
#include "llbutton.h"
#include "llresmgr.h"
#include "llinventory.h"
#include "llvoavatarself.h"
#include "llagent.h"          // gAgent
#include "llkeyframemotion.h"
#include "llfilepicker.h"
#include "lllineeditor.h"
#include "lltrans.h"
#include "lluictrlfactory.h"
#include "lluictrlfactory.h"
#include "lldatapacker.h"

extern LLAgent gAgent;
const S32 ADVANCED_VPAD = 3;

LLPreviewAnim::LLPreviewAnim(const LLSD& key)
	: LLPreview( key ),
	pMotion(NULL)
{
	mCommitCallbackRegistrar.add("PreviewAnim.Play", boost::bind(&LLPreviewAnim::play, this, _2));
}

// virtual
bool LLPreviewAnim::postBuild()
{
	childSetCommitCallback("desc", LLPreview::onText, this);
	getChild<LLLineEditor>("desc")->setPrevalidate(&LLTextValidate::validateASCIIPrintableNoPipe);
	getChild<LLTextBox>("adv_trigger")->setClickedCallback(boost::bind(&LLPreviewAnim::showAdvanced, this));
	pMoreInfoLeft = getChild<LLTextBox>("MoreInfoLeft");
    pMoreInfoRight = getChild<LLTextBox>("MoreInfoRight");
    
    // Assume that advanced stats start visible (for XUI preview tool's purposes)
    pMoreInfoLeft->setVisible(false);
    pMoreInfoRight->setVisible(false);
    // GN: Let's keep the panel size static, so no need to recalc the floater here as the original code did.
    
    
	return LLPreview::postBuild();
}

// llinventorybridge also calls into here
void LLPreviewAnim::play(const LLSD& param)
{
	const LLInventoryItem *item = getItem();

	if(item)
	{
		LLUUID itemID=item->getAssetUUID();

		std::string btn_name = param.asString();
		LLButton* btn_inuse;
		LLButton* btn_other;

		if ("Inworld" == btn_name)
		{
			btn_inuse = getChild<LLButton>("Inworld");
			btn_other = getChild<LLButton>("Locally");
		}
		else if ("Locally" == btn_name)
		{
			btn_inuse = getChild<LLButton>("Locally");
			btn_other = getChild<LLButton>("Inworld");
		}
		else
		{
			return;
		}

		if (btn_inuse)
		{
			btn_inuse->toggleState();
		}

		if (btn_other)
		{
			btn_other->setEnabled(false);
		}
		
		if (getChild<LLUICtrl>(btn_name)->getValue().asBoolean() ) 
		{
			if("Inworld" == btn_name)
			{
				gAgent.sendAnimationRequest(itemID, ANIM_REQUEST_START);
			}
			else
			{
				gAgentAvatarp->startMotion(item->getAssetUUID());
			}

			LLMotion* motion = gAgentAvatarp->findMotion(itemID);
			if (motion)
			{
				mItemID = itemID;
				mDidStart = false;
			}
		}
		else
		{
			gAgentAvatarp->stopMotion(itemID);
			gAgent.sendAnimationRequest(itemID, ANIM_REQUEST_STOP);

			if (btn_other)
			{
				btn_other->setEnabled(true);
			}
		}
	}
}

// virtual
void LLPreviewAnim::draw()
{
	LLPreview::draw();
	if (!this->mItemID.isNull())
	{
		LLMotion* motion = gAgentAvatarp->findMotion(this->mItemID);
		if (motion)
		{
			if (motion->isStopped() && this->mDidStart)
			{
				cleanup();
			}
			if(gAgentAvatarp->isMotionActive(this->mItemID) && !this->mDidStart)
			{
				const LLInventoryItem *item = getItem();
				LLMotion* motion = gAgentAvatarp->findMotion(this->mItemID);
				if (item && motion)
				{
					motion->setName(item->getName());
				}
				this->mDidStart = true;
			}
		}
	}
}

// virtual
void LLPreviewAnim::refreshFromItem()
{
    const LLInventoryItem* item = getItem();
    if (!item)
    {
        return;
    }

    // Preload motion
    pMotion = gAgentAvatarp->createMotion(item->getAssetUUID());

    LLPreview::refreshFromItem();
}

void LLPreviewAnim::cleanup()
{
	this->mItemID = LLUUID::null;
	this->mDidStart = false;
	getChild<LLUICtrl>("Inworld")->setValue(false);
	getChild<LLUICtrl>("Locally")->setValue(false);
	getChild<LLUICtrl>("Inworld")->setEnabled(true);
	getChild<LLUICtrl>("Locally")->setEnabled(true);
}

// virtual
void LLPreviewAnim::onClose(bool app_quitting)
{
	const LLInventoryItem *item = getItem();

	if(item)
	{
		gAgentAvatarp->stopMotion(item->getAssetUUID());
		gAgent.sendAnimationRequest(item->getAssetUUID(), ANIM_REQUEST_STOP);
	}
}

void LLPreviewAnim::showAdvanced()
{
    bool was_visible =  pMoreInfoLeft->getVisible(); // we only need to test for the left text being visible as we toggle visibility for both below

    if (was_visible)
    {
        pMoreInfoLeft->setVisible(false);
        pMoreInfoRight->setVisible(false);
    }
    else
    {
        pMoreInfoLeft->setVisible(true);
        pMoreInfoRight->setVisible(true);

        // set text
        if (pMotion)
        {
            pMoreInfoLeft->setTextArg("[PRIORITY]", llformat("%d", pMotion->getPriority()));
            pMoreInfoLeft->setTextArg("[DURATION]", llformat("%.2f", pMotion->getDuration()));
            pMoreInfoLeft->setTextArg("[IS_LOOP]", (pMotion->getLoop() ? LLTrans::getString("PermYes") : LLTrans::getString("PermNo")));
            pMoreInfoRight->setTextArg("[EASE_IN]", llformat("%.2f", pMotion->getEaseInDuration()));
            pMoreInfoRight->setTextArg("[EASE_OUT]", llformat("%.2f", pMotion->getEaseOutDuration()));
            pMoreInfoRight->setTextArg("[NUM_JOINTS]", llformat("%d", pMotion->getNumJointMotions()));
        }
    }
}
