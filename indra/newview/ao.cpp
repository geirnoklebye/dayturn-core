/** 
 * @file ao.cpp
 * @brief Anything concerning the Viewer Side Animation Overrider GUI
 *
 * $LicenseInfo:firstyear=2001&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2010, Linden Research, Inc.
 * Copyright (C) 2011, Zi Ree @ Second Life
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

#include "ao.h"
#include "aoengine.h"
#include "aoset.h"
#include "llcheckboxctrl.h"
#include "llcombobox.h"
#include "llnotificationsutil.h"
#include "llspinctrl.h"
#include "llviewercontrol.h"
#include "llviewerinventory.h"
#include "utilitybar.h"

// additional includes for chat notification
#include "llchat.h"
#include "llnotificationhandler.h"
#include "llnotificationmanager.h"
#include "llnotificationsutil.h"

FloaterAO::FloaterAO(const LLSD& key)
:	LLTransientDockableFloater(nullptr, true, key), LLEventTimer(10.f),
	mSetList(0),
	mSelectedSet(nullptr),
	mSelectedState(nullptr),
	mCanDragAndDrop(false),
	mImportRunning(false),
	mCurrentBoldItemID(LLUUID::null),
	mLastState(""),
	mLastName(""),
	mLastUUID(LLUUID::null),
	mMore(true)
{
	mEventTimer.stop();
}

FloaterAO::~FloaterAO()
{
}

void FloaterAO::reloading(bool yes)
{
	if (yes)
	{
		mEventTimer.start();
	}
	else
	{
		mEventTimer.stop();
	}

	mReloadCoverPanel->setVisible(yes);
	enableSetControls(!yes);
	enableStateControls(!yes);
}

bool FloaterAO::tick()
{
	// reloading took too long, probably missed the signal, so we hide the reload cover
	LL_WARNS("AOEngine") << "AO reloading timeout." << LL_ENDL;
	updateList();
	return false;
}

void FloaterAO::updateSetParameters()
{
	mOverrideSitsCheckBox->setValue(mSelectedSet->getSitOverride());
	mOverrideSitsCheckBoxSmall->setValue(mSelectedSet->getSitOverride());
	mSmartCheckBox->setValue(mSelectedSet->getSmart());
	mDisableMouselookCheckBox->setValue(mSelectedSet->getMouselookDisable());
	bool isDefault = (mSelectedSet == AOEngine::instance().getDefaultSet());
	mDefaultCheckBox->setValue(isDefault);
	mDefaultCheckBox->setEnabled(!isDefault);
	updateSmart();
}

void FloaterAO::updateAnimationList()
{
	S32 currentStateSelected = mStateSelector->getCurrentIndex();

	mStateSelector->removeall();
	onChangeAnimationSelection();

	if (!mSelectedSet)
	{
		mStateSelector->setEnabled(false);
		mStateSelector->add(getString("ao_no_animations_loaded"));
		return;
	}

	for (U32 index = 0; index < mSelectedSet->mStateNames.size(); ++index)
	{
		std::string stateName = mSelectedSet->mStateNames[index];
		AOSet::AOState* state = mSelectedSet->getStateByName(stateName);
		mStateSelector->add(stateName, state, ADD_BOTTOM, true);
	}

	enableStateControls(true);

	if (currentStateSelected == -1)
	{
		mStateSelector->selectFirstItem();
	}
	else
	{
		mStateSelector->selectNthItem(currentStateSelected);
	}

	onSelectState();
}

void FloaterAO::updateList()
{
	mReloadButton->setEnabled(true);
	mImportRunning = false;

	// Lambda provides simple Alpha sorting, note this is case sensitive.
	auto sortRuleLambda = [](const AOSet* s1, const AOSet* s2) -> bool
	{
		return s1->getName() < s2->getName();
	};

	mSetList=AOEngine::instance().getSetList();
	std::sort(mSetList.begin(), mSetList.end(), sortRuleLambda);

	// remember currently selected animation set name
	std::string currentSetName = mSetSelector->getSelectedItemLabel();

	mSetSelector->removeall();
	mSetSelectorSmall->removeall();
	mSetSelector->clear();
	mSetSelectorSmall->clear();

	mAnimationList->deleteAllItems();
	mCurrentBoldItemID = LLUUID::null;
	reloading(false);

	if (mSetList.empty())
	{
		LL_DEBUGS("AOEngine") << "empty set list" << LL_ENDL;
		mSetSelector->add(getString("ao_no_sets_loaded"));
		mSetSelectorSmall->add(getString("ao_no_sets_loaded"));
		mSetSelector->selectNthItem(0);
		mSetSelectorSmall->selectNthItem(0);
		enableSetControls(false);
		return;
	}

	// make sure we have an animation set name to display
	if (currentSetName.empty())
	{
		// selected animation set was empty, get the currently active animation set from the engine
		currentSetName = AOEngine::instance().getCurrentSetName();
		LL_DEBUGS("AOEngine") << "Current set name was empty, fetched name \"" << currentSetName << "\" from AOEngine" << LL_ENDL;

		if(currentSetName.empty())
		{
			// selected animation set was empty, get the name of the first animation set in the list
			currentSetName = mSetList[0]->getName();
			LL_DEBUGS("AOEngine") << "Current set name still empty, fetched first set's name \"" << currentSetName << "\"" << LL_ENDL;
		}
	}

	for (U32 index = 0; index < mSetList.size(); ++index)
	{
		std::string setName = mSetList[index]->getName();
		mSetSelector->add(setName, &mSetList[index], ADD_BOTTOM, true);
		mSetSelectorSmall->add(setName, &mSetList[index], ADD_BOTTOM, true);
		if (setName.compare(currentSetName) == 0)
		{
			mSelectedSet = AOEngine::instance().selectSetByName(currentSetName);
			mSetSelector->selectNthItem(index);
			mSetSelectorSmall->selectNthItem(index);
			updateSetParameters();
			updateAnimationList();
		}
	}
	enableSetControls(true);
	// not only does this addition causes crashes due to onClickReload not expecting to be called from a timer
	// it also can lead to a recursive crash since onClickReload calls this routine again so it's a matter of
	// pure luck how quickly things load and the chance of recursion goes away. It's time to do the login refresh
	// a different way
	//if (mSetSelector->getSelectedItemLabel().empty())
	//{
	//	onClickReload();
	//}
}

bool FloaterAO::postBuild()
{
	LLPanel* aoPanel = getChild<LLPanel>("animation_overrider_outer_panel");
	mMainInterfacePanel = aoPanel->getChild<LLPanel>("animation_overrider_panel");
	mSmallInterfacePanel = aoPanel->getChild<LLPanel>("animation_overrider_panel_small");
	mReloadCoverPanel = aoPanel->getChild<LLPanel>("ao_reload_cover");

	mSetSelector = mMainInterfacePanel->getChild<LLComboBox>("ao_set_selection_combo");
	mActivateSetButton = mMainInterfacePanel->getChild<LLButton>("ao_activate");
	mAddButton = mMainInterfacePanel->getChild<LLButton>("ao_add");
	mRemoveButton = mMainInterfacePanel->getChild<LLButton>("ao_remove");
	mDefaultCheckBox = mMainInterfacePanel->getChild<LLCheckBoxCtrl>("ao_default");
	mChatCheckBox = mMainInterfacePanel->getChild<LLCheckBoxCtrl>("ao_chat");
	mOverrideSitsCheckBox = mMainInterfacePanel->getChild<LLCheckBoxCtrl>("ao_sit_override");
	mSmartCheckBox = mMainInterfacePanel->getChild<LLCheckBoxCtrl>("ao_smart");
	mDisableMouselookCheckBox = mMainInterfacePanel->getChild<LLCheckBoxCtrl>("ao_disable_stands_in_mouselook");

	mStateSelector = mMainInterfacePanel->getChild<LLComboBox>("ao_state_selection_combo");
	mAnimationList = mMainInterfacePanel->getChild<LLScrollListCtrl>("ao_state_animation_list");
	mMoveUpButton = mMainInterfacePanel->getChild<LLButton>("ao_move_up");
	mMoveDownButton = mMainInterfacePanel->getChild<LLButton>("ao_move_down");
	mTrashButton = mMainInterfacePanel->getChild<LLButton>("ao_trash");
	mCycleCheckBox = mMainInterfacePanel->getChild<LLCheckBoxCtrl>("ao_cycle");
	mRandomizeCheckBox = mMainInterfacePanel->getChild<LLCheckBoxCtrl>("ao_randomize");
	mCycleTimeTextLabel = mMainInterfacePanel->getChild<LLTextBox>("ao_cycle_time_seconds_label");
	mCycleTimeSpinner = mMainInterfacePanel->getChild<LLSpinCtrl>("ao_cycle_time");

	mReloadButton = mMainInterfacePanel->getChild<LLButton>("ao_reload");
	mPreviousButton = mMainInterfacePanel->getChild<LLButton>("ao_previous");
	mNextButton = mMainInterfacePanel->getChild<LLButton>("ao_next");
	mRandomButton = mMainInterfacePanel->getChild<LLButton>("ao_random");
	mLessButton = mMainInterfacePanel->getChild<LLButton>("ao_less");

	mSetSelectorSmall = mSmallInterfacePanel->getChild<LLComboBox>("ao_set_selection_combo_small");
	mMoreButton = mSmallInterfacePanel->getChild<LLButton>("ao_more");
	mPreviousButtonSmall = mSmallInterfacePanel->getChild<LLButton>("ao_previous_small");
	mNextButtonSmall = mSmallInterfacePanel->getChild<LLButton>("ao_next_small");
	mRandomButtonSmall = mSmallInterfacePanel->getChild<LLButton>("ao_random_small");
	mOverrideSitsCheckBoxSmall = mSmallInterfacePanel->getChild<LLCheckBoxCtrl>("ao_sit_override_small");

	mSetSelector->setCommitCallback(boost::bind(&FloaterAO::onSelectSet, this));
	mSetSelector->setFocusLostCallback(boost::bind(&FloaterAO::onSelectSet, this));
	mActivateSetButton->setCommitCallback(boost::bind(&FloaterAO::onClickActivate, this));
	mAddButton->setCommitCallback(boost::bind(&FloaterAO::onClickAdd, this));
	mRemoveButton->setCommitCallback(boost::bind(&FloaterAO::onClickRemove, this));
	mDefaultCheckBox->setCommitCallback(boost::bind(&FloaterAO::onCheckDefault, this));
	mChatCheckBox->setCommitCallback(boost::bind(&FloaterAO::onCheckChat, this));
	mOverrideSitsCheckBox->setCommitCallback(boost::bind(&FloaterAO::onCheckOverrideSits, this));
	mSmartCheckBox->setCommitCallback(boost::bind(&FloaterAO::onCheckSmart, this));
	mDisableMouselookCheckBox->setCommitCallback(boost::bind(&FloaterAO::onCheckDisableStands, this));

	mAnimationList->setCommitOnSelectionChange(true);

	mStateSelector->setCommitCallback(boost::bind(&FloaterAO::onSelectState, this));
	mAnimationList->setCommitCallback(boost::bind(&FloaterAO::onChangeAnimationSelection, this));
	mMoveUpButton->setCommitCallback(boost::bind(&FloaterAO::onClickMoveUp, this));
	mMoveDownButton->setCommitCallback(boost::bind(&FloaterAO::onClickMoveDown, this));
	mTrashButton->setCommitCallback(boost::bind(&FloaterAO::onClickTrash, this));
	mCycleCheckBox->setCommitCallback(boost::bind(&FloaterAO::onCheckCycle, this));
	mRandomizeCheckBox->setCommitCallback(boost::bind(&FloaterAO::onCheckRandomize, this));
	mCycleTimeSpinner->setCommitCallback(boost::bind(&FloaterAO::onChangeCycleTime, this));

	mReloadButton->setCommitCallback(boost::bind(&FloaterAO::onClickReload, this));
	mPreviousButton->setCommitCallback(boost::bind(&FloaterAO::onClickPrevious, this));
	mNextButton->setCommitCallback(boost::bind(&FloaterAO::onClickNext, this));
	mRandomButton->setCommitCallback(boost::bind(&FloaterAO::onClickRandom, this));
	mLessButton->setCommitCallback(boost::bind(&FloaterAO::onClickLess, this));
	mOverrideSitsCheckBoxSmall->setCommitCallback(boost::bind(&FloaterAO::onCheckOverrideSitsSmall, this));

	mSetSelectorSmall->setCommitCallback(boost::bind(&FloaterAO::onSelectSetSmall, this));
	mMoreButton->setCommitCallback(boost::bind(&FloaterAO::onClickMore, this));
	mPreviousButtonSmall->setCommitCallback(boost::bind(&FloaterAO::onClickPrevious, this));
	mNextButtonSmall->setCommitCallback(boost::bind(&FloaterAO::onClickNext, this));
	mRandomButtonSmall->setCommitCallback(boost::bind(&FloaterAO::onClickRandom, this));

	updateSmart();

	AOEngine::instance().setReloadCallback(boost::bind(&FloaterAO::updateList, this));
	AOEngine::instance().setAnimationChangedCallback(boost::bind(&FloaterAO::onAnimationChanged, this, _1, _2, _3));

	onChangeAnimationSelection();
	mMainInterfacePanel->setVisible(true);
	mSmallInterfacePanel->setVisible(false);
	reloading(true);
	updateList();

	if (gSavedPerAccountSettings.getbool("UseFullAOInterface"))
	{
		onClickMore();
	}
	else
	{
		onClickLess();
	}
	mChatCheckBox->setValue(gSavedPerAccountSettings.getbool("AOChatNotifications"));

	return LLDockableFloater::postBuild();
}

void FloaterAO::enableSetControls(bool yes)
{
	mSetSelector->setEnabled(yes);
	mSetSelectorSmall->setEnabled(yes);
	mActivateSetButton->setEnabled(yes);
	mRemoveButton->setEnabled(yes);
	mDefaultCheckBox->setEnabled(yes && (mSelectedSet != AOEngine::instance().getDefaultSet()));
	mChatCheckBox->setEnabled(yes);
	mOverrideSitsCheckBox->setEnabled(yes);
	mOverrideSitsCheckBoxSmall->setEnabled(yes);
	mDisableMouselookCheckBox->setEnabled(yes);

	if (!yes)
	{
		enableStateControls(yes);
	}
}

void FloaterAO::enableStateControls(bool yes)
{
	mStateSelector->setEnabled(yes);
	mAnimationList->setEnabled(yes);
	mCycleCheckBox->setEnabled(yes);
	if (yes)
	{
		updateCycleParameters();
	}
	else
	{
		mRandomizeCheckBox->setEnabled(yes);
		mCycleTimeTextLabel->setEnabled(yes);
		mCycleTimeSpinner->setEnabled(yes);
	}
	mPreviousButton->setEnabled(yes);
	mPreviousButtonSmall->setEnabled(yes);
	mNextButton->setEnabled(yes);
	mNextButtonSmall->setEnabled(yes);
	mRandomButton->setEnabled(yes);
	mRandomButtonSmall->setEnabled(yes);

	mCanDragAndDrop = yes;
}

void FloaterAO::onOpen(const LLSD& key)
{
	UtilityBar::instance().setAOInterfaceButtonExpanded(true);
}

void FloaterAO::onClose(bool app_quitting)
{
	if (!app_quitting)
	{
		UtilityBar::instance().setAOInterfaceButtonExpanded(false);
	}
}

void FloaterAO::onSelectSet()
{
	AOSet* set = AOEngine::instance().getSetByName(mSetSelector->getSelectedItemLabel());
	if (!set)
	{
		onRenameSet();
		return;
	}
	
	// only update the interface when we actually selected a different set - FIRE-29542
	if (mSelectedSet != set)
	{
		mSelectedSet=set;

		updateSetParameters();
		updateAnimationList();
	}
}

void FloaterAO::onSelectSetSmall()
{
	// sync main set selector with small set selector
	mSetSelector->selectNthItem(mSetSelectorSmall->getCurrentIndex());

	mSelectedSet = AOEngine::instance().getSetByName(mSetSelectorSmall->getSelectedItemLabel());
	if (mSelectedSet)
	{
		updateSetParameters();
		updateAnimationList();

		// small selector activates the selected set immediately
		onClickActivate();
	}
}

void FloaterAO::onRenameSet()
{
	if (!mSelectedSet)
	{
		LL_WARNS("AOEngine") << "Rename AO set without set selected." << LL_ENDL;
		return;
	}

	std::string name = mSetSelector->getSimple();
	LLStringUtil::trim(name);

	LLUIString new_set_name = name;

	if (!name.empty())
	{
		if (
			LLTextValidate::validateASCIIPrintableNoPipe(new_set_name.getWString()) &&	// only allow ASCII
			name.find_first_of(":|") == std::string::npos)								// don't allow : or |
		{
			if (AOEngine::instance().renameSet(mSelectedSet, name))
			{
				reloading(true);
				return;
			}
		}
		else
		{
			LLSD args;
			args["AO_SET_NAME"] = name;
			LLNotificationsUtil::add("RenameAOMustBeASCII", args);
		}
	}
	mSetSelector->setSimple(mSelectedSet->getName());
}

void FloaterAO::onClickActivate()
{
	// sync small set selector with main set selector
	mSetSelectorSmall->selectNthItem(mSetSelector->getCurrentIndex());

	LL_DEBUGS("AOEngine") << "Set activated: " << mSetSelector->getSelectedItemLabel() << LL_ENDL;
	AOEngine::instance().selectSet(mSelectedSet);
}

LLScrollListItem* FloaterAO::addAnimation(const std::string& name)
{
	LLSD row;
	row["columns"][0]["column"] = "icon";
	row["columns"][0]["type"] = "icon";
	row["columns"][0]["value"] = "Inv_Animation";

	row["columns"][1]["column"] = "animation_name";
	row["columns"][1]["type"] = "text";
	row["columns"][1]["value"] = name;

	return mAnimationList->addElement(row);
}

void FloaterAO::onSelectState()
{
	mAnimationList->deleteAllItems();
	mCurrentBoldItemID = LLUUID::null;
	mAnimationList->setCommentText(getString("ao_no_animations_loaded"));
	mAnimationList->setEnabled(false);

	onChangeAnimationSelection();

	if (!mSelectedSet)
	{
		return;
	}

	mSelectedState = mSelectedSet->getStateByName(mStateSelector->getSelectedItemLabel());
	if (!mSelectedState)
	{
		return;
	}

	mSelectedState = (AOSet::AOState*)mStateSelector->getCurrentUserdata();
	if (mSelectedState->mAnimations.size())
	{
		for (U32 index = 0; index < mSelectedState->mAnimations.size(); ++index)
		{
			LLScrollListItem* item = addAnimation(mSelectedState->mAnimations[index].mName);
			if (item)
			{
				item->setUserdata(&mSelectedState->mAnimations[index].mInventoryUUID);
			}
		}

		mAnimationList->setCommentText("");
		mAnimationList->setEnabled(true);
	}

	mCycleCheckBox->setValue(mSelectedState->mCycle);
	mRandomizeCheckBox->setValue(mSelectedState->mRandom);
	mCycleTimeSpinner->setValue(mSelectedState->mCycleTime);

	updateCycleParameters();
	// see if the new state contains the current anim and get it bolded if so
	LL_DEBUGS("AOEngine") << "Trying to reinstate: " << mLastState << " / " << mLastName << LL_ENDL;
	onAnimationChanged(mLastUUID, mLastState, mLastName);
}

void FloaterAO::onClickReload()
{
	reloading(true);

	mSelectedSet = nullptr;
	mSelectedState = nullptr;

	AOEngine::instance().reload(false);
	updateList();
}

void FloaterAO::onClickAdd()
{
	LLNotificationsUtil::add("NewAOSet", LLSD(), LLSD(), boost::bind(&FloaterAO::newSetCallback, this, _1, _2));
}

bool FloaterAO::newSetCallback(const LLSD& notification, const LLSD& response)
{
	std::string newSetName = response["message"].asString();
	S32 option = LLNotificationsUtil::getSelectedOption(notification, response);

	LLStringUtil::trim(newSetName);

	LLUIString new_set_name = newSetName;

	if (newSetName.empty())
	{
		return false;
	}
	else if (
		!LLTextValidate::validateASCIIPrintableNoPipe(new_set_name.getWString()) ||		// only allow ASCII
		newSetName.find_first_of(":|") != std::string::npos)							// don't allow : or |
	{
		LLSD args;
		args["AO_SET_NAME"] = newSetName;
		LLNotificationsUtil::add("NewAOCantContainNonASCII", args);
		return false;
	}

	if (option == 0)
	{
		if (AOEngine::instance().addSet(newSetName).notNull())
		{
			reloading(true);
			return true;
		}
	}
	return false;
}

void FloaterAO::onClickRemove()
{
	if (!mSelectedSet)
	{
		return;
	}

	LLSD args;
	args["AO_SET_NAME"] = mSelectedSet->getName();
	LLNotificationsUtil::add("RemoveAOSet", args, LLSD(), boost::bind(&FloaterAO::removeSetCallback, this, _1, _2));
}

bool FloaterAO::removeSetCallback(const LLSD& notification, const LLSD& response)
{
	S32 option = LLNotificationsUtil::getSelectedOption(notification, response);

	if (option ==0 )
	{
		if (AOEngine::instance().removeSet(mSelectedSet))
		{
			reloading(true);
			// to prevent snapping back to deleted set
			mSetSelector->removeall();
			mSetSelectorSmall->removeall();
			// visually indicate there are no items left
			mSetSelector->clear();
			mSetSelectorSmall->clear();
			mAnimationList->deleteAllItems();
			mCurrentBoldItemID = LLUUID::null;
			return true;
		}
	}
	return false;
}

void FloaterAO::onCheckDefault()
{
	if (mSelectedSet)
	{
		AOEngine::instance().setDefaultSet(mSelectedSet);
	}
}

void FloaterAO::onCheckChat()
{
	bool value = !(gSavedPerAccountSettings.getbool("AOChatNotifications"));
	mChatCheckBox->setValue(value);
	gSavedPerAccountSettings.setbool("AOChatNotifications",value);
}

void FloaterAO::onCheckOverrideSits()
{
	mOverrideSitsCheckBoxSmall->setValue(mOverrideSitsCheckBox->getValue());
	if (mSelectedSet)
	{
		AOEngine::instance().setOverrideSits(mSelectedSet, mOverrideSitsCheckBox->getValue().asBoolean());
	}
	updateSmart();
}

void FloaterAO::onCheckOverrideSitsSmall()
{
	mOverrideSitsCheckBox->setValue(mOverrideSitsCheckBoxSmall->getValue());
	onCheckOverrideSits();
}

void FloaterAO::updateSmart()
{
	mSmartCheckBox->setEnabled(mOverrideSitsCheckBox->getValue());
}

void FloaterAO::onCheckSmart()
{
	if (mSelectedSet)
	{
		AOEngine::instance().setSmart(mSelectedSet, mSmartCheckBox->getValue().asBoolean());
	}
}

void FloaterAO::onCheckDisableStands()
{
	if (mSelectedSet)
	{
		AOEngine::instance().setDisableStands(mSelectedSet, mDisableMouselookCheckBox->getValue().asBoolean());
	}
}

void FloaterAO::onChangeAnimationSelection()
{
	std::vector<LLScrollListItem*> list = mAnimationList->getAllSelected();
	LL_DEBUGS("AOEngine") << "Selection count: " << list.size() << LL_ENDL;

	bool resortEnable = false;
	bool trashEnable = false;

	// Linden Lab bug: scroll lists still select the first item when you click on them, even when they are disabled.
	// The control does not memorize it's enabled/disabled state, so mAnimationList->mEnabled() doesn't seem to work.
	// So we need to safeguard against it.
	if (!mCanDragAndDrop)
	{
		mAnimationList->deselectAllItems();
		LL_DEBUGS("AOEngine") << "Selection count now: " << list.size() << LL_ENDL;
	}
	else if (list.size() > 0)
	{
		if (list.size() == 1)
		{
			resortEnable = true;
		}
		trashEnable = true;
	}

	mMoveDownButton->setEnabled(resortEnable);
	mMoveUpButton->setEnabled(resortEnable);
	mTrashButton->setEnabled(trashEnable);
}

void FloaterAO::onClickMoveUp()
{
	if (!mSelectedState)
	{
		return;
	}

	std::vector<LLScrollListItem*> list = mAnimationList->getAllSelected();
	if (list.size() != 1)
	{
		return;
	}

	S32 currentIndex = mAnimationList->getFirstSelectedIndex();
	if (currentIndex == -1)
	{
		return;
	}

	if (AOEngine::instance().swapWithPrevious(mSelectedState, currentIndex))
	{
		mAnimationList->swapWithPrevious(currentIndex);
	}
}

void FloaterAO::onClickMoveDown()
{
	if (!mSelectedState)
	{
		return;
	}

	std::vector<LLScrollListItem*> list = mAnimationList->getAllSelected();
	if (list.size() != 1)
	{
		return;
	}

	S32 currentIndex = mAnimationList->getFirstSelectedIndex();
	if (currentIndex >= (mAnimationList->getItemCount() - 1))
	{
		return;
	}

	if (AOEngine::instance().swapWithNext(mSelectedState, currentIndex))
	{
		mAnimationList->swapWithNext(currentIndex);
	}
}

void FloaterAO::onClickTrash()
{
	if (!mSelectedState)
	{
		return;
	}

	std::vector<LLScrollListItem*> list = mAnimationList->getAllSelected();
	if (list.size() == 0)
	{
		return;
	}

	for (S32 index = list.size() - 1; index != -1; --index)
	{
		AOEngine::instance().removeAnimation(mSelectedSet, mSelectedState, mAnimationList->getItemIndex(list[index]));
	}

	mAnimationList->deleteSelectedItems();
	mCurrentBoldItemID = LLUUID::null;
}

void FloaterAO::updateCycleParameters()
{
	bool yes = mCycleCheckBox->getValue().asBoolean();
	mRandomizeCheckBox->setEnabled(yes);
	mCycleTimeTextLabel->setEnabled(yes);
	mCycleTimeSpinner->setEnabled(yes);
}

void FloaterAO::onCheckCycle()
{
	if (mSelectedState)
	{
		bool yes = mCycleCheckBox->getValue().asBoolean();
		AOEngine::instance().setCycle(mSelectedState, yes);
		updateCycleParameters();
	}
}

void FloaterAO::onCheckRandomize()
{
	if (mSelectedState)
	{
		AOEngine::instance().setRandomize(mSelectedState, mRandomizeCheckBox->getValue().asBoolean());
	}
}

void FloaterAO::onChangeCycleTime()
{
	if (mSelectedState)
	{
		AOEngine::instance().setCycleTime(mSelectedState, mCycleTimeSpinner->getValueF32());
	}
}

void FloaterAO::onClickPrevious()
{
	AOEngine::instance().cycle(AOEngine::CyclePrevious);
}

void FloaterAO::onClickNext()
{
	AOEngine::instance().cycle(AOEngine::CycleNext);
}

void FloaterAO::onClickRandom()
{
	AOEngine::instance().cycle(AOEngine::CycleAny);
}

void FloaterAO::onClickMore()
{
	LLRect fullSize = gSavedPerAccountSettings.getRect("floater_rect_animation_overrider_full");

	if (fullSize.getHeight() < getMinHeight())
	{
		fullSize.setOriginAndSize(fullSize.mLeft, fullSize.mBottom, fullSize.getWidth(), getRect().getHeight());
	}

	if (fullSize.getWidth() < getMinWidth())
	{
		fullSize.setOriginAndSize(fullSize.mLeft, fullSize.mBottom, getRect().getWidth(), fullSize.getHeight());
	}

	mMore = true;

	mSmallInterfacePanel->setVisible(false);
	mMainInterfacePanel->setVisible(true);
	setCanResize(true);

	gSavedPerAccountSettings.setbool("UseFullAOInterface", true);

	reshape(getRect().getWidth(), fullSize.getHeight());
}

void FloaterAO::onClickLess()
{
	LLRect fullSize = getRect();
	LLRect smallSize = mSmallInterfacePanel->getRect();
	smallSize.setLeftTopAndSize(0, 0, smallSize.getWidth(), smallSize.getHeight() + getHeaderHeight());

	gSavedPerAccountSettings.setRect("floater_rect_animation_overrider_full", fullSize);

	mMore = false;

	mSmallInterfacePanel->setVisible(true);
	mMainInterfacePanel->setVisible(false);
	setCanResize(false);

	gSavedPerAccountSettings.setbool("UseFullAOInterface", false);

	reshape(getRect().getWidth(), smallSize.getHeight());

	// save current size and position
	gSavedPerAccountSettings.setRect("floater_rect_animation_overrider_full", fullSize);
}

void FloaterAO::onAnimationChanged(const LLUUID& animation, const std::string state, const std::string name)
{
	LL_DEBUGS("AOEngine") << "Received animation change to " << animation << LL_ENDL;
	
	// BEWARE: This routine now has very little resemblance with the original. Be very cautious with merges.

	// This is the relocated fix to make sure the UI is populated if login happens with the full size
	// panel selected - the original of this (calling onClickReload from updateList() ) caused problems
	// with recursive crashes (because onClickReload calls back to updateList) and instance tracker
	// exits because onClickReload was getting called from timer and didn't expect to be (since it calls
	// clear(false) to assert it's not in a timer but it actually was)
	if (mSetSelector->getSelectedItemLabel().empty())
	{
		updateList();
	}

	// we have to do our own de-dupe check because we can't rely on the bold item - it's possible that the
	// anim concerned isn't from the group being displayed so there's no match in the currently displayed scroll list
	if (gSavedPerAccountSettings.getbool("AOChatNotifications") && animation != LLUUID::null && animation != mLastUUID)
	{
		LLChat chat;
		chat.mFromID = LLUUID::null;
		chat.mSourceType = CHAT_SOURCE_SYSTEM;
		chat.mText = "Starting animation: " + state + " / " + name;

		LLSD none;
		LLNotificationsUI::LLNotificationManager::instance().onChat(chat, none);

		// these are specifically for de-duping the notifications so we don't update for nulls
		mLastName = name;
		mLastState = state;
		mLastUUID = animation;
	}

	// why do we have no LLScrollListCtrl::getItemByUserdata() ? -Zi
	std::vector<LLScrollListItem*> item_list = mAnimationList->getAllData();
	std::vector<LLScrollListItem*>::const_iterator iter;
	
	// after trying various ways to make the unbold smart, I'm settling on a brute force approach - go through the whole
	// list and clear every item except the one we want bold
		
	for (iter = item_list.begin(); iter != item_list.end(); ++iter)
	{
		LLScrollListItem* item = *iter;
		LLUUID* id = (LLUUID*)item->getUserdata();
		// compare uuids rather than memory pointers so that we can get a match if someone displays a different state and
		// then comes back to the original state with an item in it that needs to be boldened. The original pointer is no
		// longer valid because the scroll list has been rebuilt at least twice, but uuids won't have changed
		//
		// beware - UserData can be null sometimes now that we don't take an early exit above, so checks are needed
		LLScrollListText* column = (LLScrollListText*)item->getColumn(1);
		if (id && *id == animation)
		{
			column->setFontStyle(LLFontGL::BOLD);
		}
		else
		{
			column->setFontStyle(LLFontGL::NORMAL);
		}
	}
	mCurrentBoldItemID = animation;
}

// virtual
bool FloaterAO::handleDragAndDrop(S32 x, S32 y, MASK mask, bool drop, EDragAndDropType type, void* data,
									EAcceptance* accept, std::string& tooltipMsg)
{
	// no drag & drop on small interface
	if (!mMore)
	{
		tooltipMsg = getString("ao_dnd_only_on_full_interface");
		*accept = ACCEPT_NO;
		return true;
	}

	LLInventoryItem* item = (LLInventoryItem*)data;

	if (type == DAD_NOTECARD)
	{
		if (mImportRunning)
		{
			*accept = ACCEPT_NO;
			return true;
		}
		*accept = ACCEPT_YES_SINGLE;
		if (item && drop)
		{
			if (AOEngine::instance().importNotecard(item))
			{
				reloading(true);
				mReloadButton->setEnabled(false);
				mImportRunning = true;
			}
		}
	}
	else if (type == DAD_ANIMATION)
	{
		if (!drop && (!mSelectedSet || !mSelectedState || !mCanDragAndDrop))
		{
			*accept = ACCEPT_NO;
			return true;
		}
		*accept = ACCEPT_YES_MULTI;
		if (item && drop)
		{
			if (AOEngine::instance().addAnimation(mSelectedSet, mSelectedState, item))
			{
				addAnimation(item->getName());

				// TODO: this would be the right thing to do, but it blocks multi drop
				// before final release this must be resolved
				reloading(true);
			}
		}
	}
	else
	{
		*accept = ACCEPT_NO;
	}

	return true;
}
