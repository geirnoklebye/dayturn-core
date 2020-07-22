/**
 * @file exogroupmutelist.h
 * @brief Persistently stores groups to ignore.
 *
 * $LicenseInfo:firstyear=2012&license=viewerlgpl$
 * Copyright (C) 2012 Katharine Berry
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * $/LicenseInfo$
 */

#ifndef EXO_GROUPMUTELIST_H
#define EXO_GROUPMUTELIST_H

#include "lluuid.h"
#include "llmutelist.h"

class exoGroupMuteList : public LLSingleton<exoGroupMuteList>, LLMuteListObserver
{
	LLSINGLETON(exoGroupMuteList);
	~exoGroupMuteList();

public:
	bool isMuted(const LLUUID &group) const;
	void add(const LLUUID &group);
	void remove(const LLUUID &group);
	bool loadMuteList();
	//KKA-743 called by mute list
	virtual void onChange();
	std::string getFilePath() const;

private:
	bool saveMuteList();
	
	std::set<LLUUID> mMuted;

	// <FS:Ansariel> Server-side storage
	std::string getMutelistString(const LLUUID& group) const;
	LLUUID extractUUID(std::string muteListString);
};

#endif
