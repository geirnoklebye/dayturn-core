/* Copyright (c) 2009
 *
 * Modular Systems. All rights reserved.
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
 *   3. Neither the name Modular Systems nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MODULAR SYSTEMS AND CONTRIBUTORS “AS IS”
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
#include "llsingleton.h"

#include "llfloater.h"
#include "llsingleton.h"
#include "lluuid.h"
#include "llselectmgr.h"
#include "llstring.h"
#include "llframetimer.h"
#include "lllistcontextmenu.h"

class LLTextBox;
class LLScrollListCtrl;
class LLViewerRegion;

struct ObjectDetails
{
	LLUUID id;
	std::string name;
	std::string desc;
	LLUUID owner_id;
	LLUUID group_id;
};

class FSAreaSearch : public LLSingleton<FSAreaSearch>, public LLFloater
{
public:
	FSAreaSearch(const LLSD &);
	virtual ~FSAreaSearch();

	/*virtual*/ BOOL postBuild();

	void callbackLoadOwnerName(const LLUUID& id, const std::string& full_name);
	void processObjectPropertiesFamily(LLMessageSystem* msg);

	std::string getObjectName(const LLUUID &id) { return mObjectDetails[id].name; }

	static bool enableSelection(LLScrollListCtrl *ctrl, const LLSD &userdata);
	static bool handleSelection(LLScrollListCtrl *ctrl, const LLSD &userdata);

	class ContextMenu : public LLListContextMenu, public LLSingleton<ContextMenu>
	{
	public:
		ContextMenu();
		/*virtual*/ void show(LLView *view, const uuid_vec_t &uuids, S32 x, S32 y);
		/*virtual*/ LLContextMenu *createMenu();

	protected:
		FSAreaSearch *mFloater;
		LLScrollListCtrl *mParent;
	};

private:
	void results();
	void checkRegion();
	void cancel();
	void search();
	void onCommitLine(class LLLineEditor* line, void* user_data);
	void requestIfNeeded(class LLViewerObject *objectp);

	void onDoubleClick();
	void onRightClick(S32 x, S32 y);

	enum OBJECT_COLUMN_ORDER
	{
		LIST_OBJECT_NAME,
		LIST_OBJECT_DESC,
		LIST_OBJECT_OWNER,
		LIST_OBJECT_GROUP
	};

	S32 mRequested;

	LLTextBox* mCounterText;
	LLScrollListCtrl* mResultList;
	LLFrameTimer mLastUpdateTimer;
	LLObjectSelectionHandle mSelectionHandle;

	std::map<LLUUID, ObjectDetails> mObjectDetails;

	std::string mSearchedName;
	std::string mSearchedDesc;
	std::string mSearchedOwner;
	std::string mSearchedGroup;

	LLViewerRegion* mLastRegion;
	
	class FSParcelChangeObserver;
	friend class FSParcelChangeObserver;
	FSParcelChangeObserver*	mParcelChangedObserver;
};
