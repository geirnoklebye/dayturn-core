/** 
 * @file llcompilequeue.h
 * @brief LLCompileQueue class header file
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

#ifndef LL_LLCOMPILEQUEUE_H
#define LL_LLCOMPILEQUEUE_H

#include "llinventory.h"
#include "llviewerobject.h"
#include "lluuid.h"

#include "llfloater.h"
#include "llscrolllistctrl.h"

#include "llevents.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class LLFloaterScriptQueue
//
// This class provides a mechanism of adding objects to a list that
// will go through and execute action for the scripts on each object. The
// objects will be accessed serially and the scripts may be
// manipulated in parallel. For example, selecting two objects each
// with three scripts will result in the first object having all three
// scripts manipulated.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class LLFloaterScriptQueue : public LLFloater/*, public LLVOInventoryListener*/
{
public:
	LLFloaterScriptQueue(const LLSD& key);
	virtual ~LLFloaterScriptQueue();

	/*virtual*/ bool postBuild();
	
	void setMono(bool mono) { mMono = mono; }
	void setFull(bool full) { mFull = full; }
	bool getFull() { return mFull; }
	void setDeleteOnly(bool delete_only) { mDeleteOnly = delete_only; }
	bool getDeleteOnly() { return mDeleteOnly; }
	void setDeleteName(std::string deletename) { mDeleteName = deletename; }
	std::string getDeleteName() { return mDeleteName; }
	
	// addObject() accepts an object id.
	void addObject(const LLUUID& id, std::string name);

	// start() returns true if queue started or notification raised,
	// FALSE only if starting a non-notification action failed
	// (previously it passed back the return from startQueue() directly)
	bool start();
	
    void addProcessingMessage(const std::string &message, const LLSD &args);
    void addStringMessage(const std::string &message);

    std::string getStartString() const { return mStartString; }

  void setWaiting(bool setwaiting) { mAwaitingMessage = setwaiting; }  
  bool getWaiting() { return mAwaitingMessage; }  
  void setIsMono(bool setismono) { mIsMono = setismono; setismono ? mMonoCount++ : mLSLCount++; }  
  bool getIsMono() { return mIsMono; }  
  void setIsRunning(bool setisrunning) { mIsRunning = setisrunning; if (setisrunning) mRunningCount++; }  
  bool getIsRunning() { return mIsRunning; } 
  void clearScriptTotals() { mMonoCount=0; mLSLCount=0; mRunningCount=0; }
  U32 getMonoCount() { return mMonoCount; }
  U32 getLSLCount() { return mLSLCount; }
  U32 getRunningCount() { return mRunningCount; } 
    
protected:
	static void onCloseBtn(void* user_data);

	// this does the actual starting immediately for informational operations or
	// after confirmation for modifying/destructive operations
	bool confirmedStart();

	bool onScriptQueueConfirmation(const LLSD& notification, const LLSD& response);

  void onClickCopyToClipboard();
	
	// returns true if this is done
	bool isDone() const;

	virtual bool startQueue() = 0;

	void setStartString(const std::string& s) { mStartString = s; }

protected:
	// UI
	LLScrollListCtrl* mMessages;
	LLButton* mCloseBtn;

	// Object Queue
	struct ObjectData
	{
		LLUUID mObjectId;
		std::string mObjectName;
	};
	typedef std::vector<ObjectData> object_data_list_t;

	object_data_list_t mObjectList;
	LLUUID mCurrentObjectID;
	bool mDone;

	std::string mStartString;
	bool mMono;
	bool mFull;
  bool mAwaitingMessage;
  bool mIsMono;
  bool mIsRunning;
  bool mDeleteOnly;
  std::string mDeleteName;
  U32 mMonoCount;
  U32 mLSLCount;
  U32 mRunningCount; // we don't need a not running count - it's (mono+lsl) minus running

    typedef boost::function<bool(const LLPointer<LLViewerObject> &, LLInventoryObject*, LLEventPump &)>   fnQueueAction_t;
    static void objectScriptProcessingQueueCoro(std::string action, LLHandle<LLFloaterScriptQueue> hfloater, object_data_list_t objectList, fnQueueAction_t func);

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class LLFloaterCompileQueue
//
// This script queue will recompile each script.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct LLCompileQueueData
{
	LLUUID mQueueID;
	LLUUID mItemId;
	LLCompileQueueData(const LLUUID& q_id, const LLUUID& item_id) :
		mQueueID(q_id), mItemId(item_id) {}
};

class LLFloaterCompileQueue : public LLFloaterScriptQueue
{
	friend class LLFloaterReg;
public:
	
	void experienceIdsReceived( const LLSD& content );
	bool hasExperience(const LLUUID& id)const;

protected:
	LLFloaterCompileQueue(const LLSD& key);
	virtual ~LLFloaterCompileQueue();
	
	virtual bool startQueue();

    static bool processScript(LLHandle<LLFloaterCompileQueue> hfloater, const LLPointer<LLViewerObject> &object, LLInventoryObject* inventory, LLEventPump &pump);

    //bool checkAssetId(const LLUUID &assetId);
    static void handleHTTPResponse(std::string pumpName, const LLSD &expresult);
    static void handleScriptRetrieval(const LLUUID& assetId, LLAssetType::EType type, void* userData, S32 status, LLExtStat extStatus);

private:
    static void processExperienceIdResults(LLSD result, LLUUID parent);
    //uuid_list_t mAssetIds;  // list of asset IDs processed.
	uuid_list_t mExperienceIds;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class LLFloaterResetQueue
//
// This script queue will reset each script.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class LLFloaterResetQueue : public LLFloaterScriptQueue
{
	friend class LLFloaterReg;
protected:
	LLFloaterResetQueue(const LLSD& key);
	virtual ~LLFloaterResetQueue();
	
    static bool resetObjectScripts(LLHandle<LLFloaterScriptQueue> hfloater, const LLPointer<LLViewerObject> &object, LLInventoryObject* inventory, LLEventPump &pump);

    virtual bool startQueue();
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class LLFloaterRunQueue
//
// This script queue will set each script as running.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class LLFloaterRunQueue : public LLFloaterScriptQueue
{
	friend class LLFloaterReg;
protected:
	LLFloaterRunQueue(const LLSD& key);
	virtual ~LLFloaterRunQueue();

    static bool runObjectScripts(LLHandle<LLFloaterScriptQueue> hfloater, const LLPointer<LLViewerObject> &object, LLInventoryObject* inventory, LLEventPump &pump);

    virtual bool startQueue();
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class LLFloaterNotRunQueue
//
// This script queue will set each script as not running.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class LLFloaterNotRunQueue : public LLFloaterScriptQueue
{
	friend class LLFloaterReg;
protected:
	LLFloaterNotRunQueue(const LLSD& key);
	virtual ~LLFloaterNotRunQueue();
	
    static bool stopObjectScripts(LLHandle<LLFloaterScriptQueue> hfloater, const LLPointer<LLViewerObject> &object, LLInventoryObject* inventory, LLEventPump &pump);

    virtual bool startQueue();
};

// Kokua addition

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class LLFloaterLocateQueue
//
// This script queue locates each script and then optionally gets more information.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class LLFloaterLocateQueue : public LLFloaterScriptQueue
{
	friend class LLFloaterReg;
	
public:
  static void processScriptRunningReply(LLMessageSystem* msg);
	
protected:
	LLFloaterLocateQueue(const LLSD& key);
	virtual ~LLFloaterLocateQueue();
	
    static bool locateObjectScripts(LLHandle<LLFloaterScriptQueue> hfloater, const LLPointer<LLViewerObject> &object, LLInventoryObject* inventory, LLEventPump &pump);

    virtual bool startQueue();
};

// Kokua addition

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class LLFloaterDeleteQueue
//
// This script queue will delete each script.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class LLFloaterDeleteQueue : public LLFloaterScriptQueue
{
	friend class LLFloaterReg;
protected:
	LLFloaterDeleteQueue(const LLSD& key);
	virtual ~LLFloaterDeleteQueue();
	
    static bool deleteObjectScripts(LLHandle<LLFloaterScriptQueue> hfloater, const LLPointer<LLViewerObject> &object, LLInventoryObject* inventory, LLEventPump &pump);

    virtual bool startQueue();
};

#endif // LL_LLCOMPILEQUEUE_H
