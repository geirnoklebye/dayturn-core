/** 
 * @file llviewernetwork.h
 * @author James Cook
 * @brief Networking constants and globals for viewer.
 *
 * $LicenseInfo:firstyear=2006&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2010, Linden Research, Inc.
 * With modifications Copyright (C) 2012, arminweatherwax@lavabit.com
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

#ifndef LL_LLVIEWERNETWORK_H
#define LL_LLVIEWERNETWORK_H

#include "llxmlnode.h"
                                                                                                       
#include <boost/function.hpp>
#include <boost/signals2.hpp>
                                                                                                       
//Kokua: for llviewernetwork_test
#define KNOWN_GRIDS_SIZE 3
#define AGNI "Second Life"
#define ADITI "Second Life Beta"

#define GRID_LABEL_VALUE "gridname"
#define GRID_NICK_VALUE "gridnick"
//#define GRID_ID_VALUE "grid_login_id"
#define GRID_LOGIN_URI_VALUE "loginuri"
#define GRID_HELPER_URI_VALUE "helperuri"
#define GRID_LOGIN_PAGE_VALUE "loginpage"
#define GRID_REGISTER_NEW_ACCOUNT "register"
#define GRID_FORGOT_PASSWORD "password"
#define MAINGRID "login.agni.lindenlab.com"
class GridInfoRequestResponder;

// <AW opensim>
struct GridEntry
{
	LLSD grid;
	LLXMLNodePtr info_root;
	bool set_current;
	std::string last_http_error;
};

/// Exception thrown when a grid is not valid
class LLInvalidGridName
{
public:
	LLInvalidGridName(std::string grid) : mGrid(grid)
	{
	}
	std::string name() { return mGrid; }
protected:
	std::string mGrid;
};

// </AW opensim>


/**
 * @brief A singleton class to manage the grids available to the viewer.
 *
 * This class maintains several properties for each known grid, and provides
 * interfaces for obtaining each of these properties given a specified
 * grid.  Grids are specified by either of two identifiers, each of which
 * must be unique among all known grids:
 * - grid name : DNS name for the grid
 * - grid id   : a short form (conventionally a single word) 
 *
 * This class maintains the currently selected grid, and provides short
 * form accessors for each of the properties of the selected grid.
 **/
class LLGridManager : public LLSingleton<LLGridManager>
{
  public:
	// <AW opensim>
	typedef enum 
	{
		FETCH,
		FETCHTEMP,
		SYSTEM,
		RETRY,
		LOCAL,
		FINISH,
		TRYLEGACY,
		FAIL,
		REMOVE
	} AddState;
public:
	LLGridManager();
	~LLGridManager();
	
// <AW opensim>
	void initGrids();
	void initSystemGrids();
	void initGridList(std::string grid_file, AddState state);
	void initCmdLineGrids();
	void resetGrids();

	//@}
	void addGrid(const std::string& loginuri);
	bool isReadyToLogin(){return mReadyToLogin;}
	void removeGrid(const std::string& grid);
	void reFetchGrid() { reFetchGrid(mGrid, true); }
	void reFetchGrid(const std::string& grid, bool set_current = false);
	
	/* ================================================================
	 * @name Grid Identifiers 
	std::map<std::string, std::string> getKnownGrids();
	 * The id is a short form (typically one word) grid name,
	 * It should be used in URL path elements or parameters
	 *
	 * Each grid also has a "label", intented to be a user friendly
	 * descriptive form (it is used in the login panel grid menu, for example).
	 */
	/// Return the name of a grid, given either its name or its id
	std::string getGrid( const std::string &grid );

	// this was getGridInfo - renamed to avoid ambiguity with the OpenSim grid_info
	void getGridData(const std::string& grid, LLSD &grid_info);
	void getGridData(LLSD &grid_info) { getGridData(mGrid, grid_info); }
// </AW opensim>	

	/// Get the id (short form selector) for the selected grid
	std::string getGridId() { return getGridId(mGrid); }

	/// Get the user-friendly long form descriptor for a given grid
	std::string getGridLabel(const std::string& grid);
	
	/// Get the user-friendly long form descriptor for the selected grid
	std::string getGridLabel() { return getGridLabel(mGrid); }

	//get the grid label e.g. "Second Life"
	std::map<std::string, std::string> getKnownGrids();
	std::string getGridLabel() { return mGridList[mGrid][GRID_LABEL_VALUE]; }
	//get the grid nick e.g. "agni"
	std::string getGridNick() { return mGridList[mGrid][GRID_NICK_VALUE].asString(); }
	//get the grid e.g. "login.agni.lindenlab.com"
	 * @{
// <FS:AW  grid management>
	// get the first (and very probably only) login URI of a specified grid
	std::string getLoginURI(const std::string& grid);
// </FS:AW  grid management>
	// get the Login URIs of the current grid
	 * A grid may have multple login uris, so they are returned as a vector.
	 */
	void getLoginURIs(const std::string& grid, std::vector<std::string>& uris);
	
	/// Get the login uris for the selected grid
	void getLoginURIs(std::vector<std::string>& uris);
	
	/// Get the URI for webdev help functions for the specified grid
	std::string getHelperURI(const std::string& grid);

	/// Get the URI for webdev help functions for the selected grid
	std::string getHelperURI() { return getHelperURI(mGrid); }

	/// Get the url of the splash page to be displayed prior to login
	std::string getLoginPage(const std::string& grid_name);

	/// Get the URI for the login splash page for the selected grid
	std::string getLoginPage();
	std::string getGridLoginID() { return mGridList[mGrid][GRID_VALUE]; }
	// was ://	std::string getGridLoginID() { return mGridList[mGrid][GRID_ID_VALUE]; }
	// however we already have that in GRID_VALUE

	std::string trimHypergrid(const std::string& trim);


	/// Get an array of the login types supported by the grid
	void getLoginIdentifierTypes(LLSD& idTypes);
	/**< the types are "agent" and "avatar";
	 * one means single-name (someone Resident) accounts and other first/last name accounts
	 * I am not sure which is which
	 */

	//@}

	/* ================================================================
	 * @name URL Construction Properties
	 * @{
	 */

	/// Return the slurl prefix (everything up to but not including the region) for a given grid
	std::string getSLURLBase(const std::string& grid);

	/// Return the slurl prefix (everything up to but not including the region) for the selected grid
	std::string getSLURLBase() { return getSLURLBase(mGrid); }
	
	/// Return the application URL prefix for the given grid
	std::string getAppSLURLBase(const std::string& grid);

	/// Return the application URL prefix for the selected grid
	std::string getAppSLURLBase() { return getAppSLURLBase(mGrid); }	

	bool hasGrid(const std::string& grid){ return mGridList.has(grid); }
	bool isTemporary(){ return mGridList[mGrid].has("FLAG_TEMPORARY"); }
	bool isTemporary(const std::string& grid){ return mGridList[grid].has("FLAG_TEMPORARY"); }

	// tell if we got this from a Hypergrid SLURL
	bool isHyperGrid(const std::string& grid) { return mGridList[grid].has("HG"); }

	// tell if we know how to acess this grid via Hypergrid
	std::string getGatekeeper() { return getGatekeeper(mGrid); }
	std::string getGatekeeper(const std::string& grid) { return mGridList[grid].has("gatekeeper") ? mGridList[grid]["gatekeeper"].asString() : std::string(); }
	 * -# The grid used most recently (setting CurrentGrid)
	 * -# The main grid (Agni)
	 */




	std::string getGridByProbing( const std::string &probe_for, bool case_sensitive = false);
	std::string getGridByLabel( const std::string &grid_label, bool case_sensitive = false);
	std::string getGridByGridNick( const std::string &grid_nick, bool case_sensitive = false);
	std::string getGridByHostName( const std::string &host_name, bool case_sensitive = false);
	std::string getGridByAttribute(const std::string &attribute, const std::string &attribute_value, bool case_sensitive );
// </AW opensim>
	bool isSystemGrid(const std::string& grid);

	/// Is the selected grid one of the hard-coded default grids (Agni or Aditi)
	bool isSystemGrid() { return isSystemGrid(mGrid); }

// <FS:AW  grid management>
	typedef boost::function<void(bool success)> grid_list_changed_callback_t;
	typedef boost::signals2::signal<void(bool success)> grid_list_changed_signal_t;

	boost::signals2::connection addGridListChangedCallback(grid_list_changed_callback_t cb);
	grid_list_changed_signal_t	mGridListChangedSignal;
// <FS:AW  grid management>
	
// <AW opensim>
	bool isInSLMain();
	bool isInSLBeta();
	bool isInOpenSim();
	void saveGridList();
	
private:
	friend class GridInfoRequestResponder;
	void addGrid(GridEntry* grid_info, AddState state);
	void incResponderCount(){++mResponderCount;}
	void decResponderCount(){--mResponderCount;}
	void gridInfoResponderCB(GridEntry* grid_data);

	void setGridData(const LLSD &grid_info) { mGridList[mGrid]=grid_info; }


	void updateIsInProductionGrid();

	// helper function for adding the hard coded grids
	void addSystemGrid(const std::string& label, 
					   const std::string& name,
					   const std::string& nick,
					   const std::string& login, 
					   const std::string& helper,
					   const std::string& login_page);
	
	
	std::string mGrid;
	std::string mGridFile;
	LLSD mGridList;
	LLSD mConnectedGrid;
	bool mIsInSLMain;
	bool mIsInSLBeta;
	bool mIsInOpenSim;
	int mResponderCount;
	bool mReadyToLogin;
	bool mCommandLineDone;
};

const S32 MAC_ADDRESS_BYTES = 6;

#endif
