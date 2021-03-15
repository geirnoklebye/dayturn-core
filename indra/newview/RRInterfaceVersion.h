/** 
 * @file RRInterfaceVersion.h
 * @author Marine Kelley
 * @brief RLV Version, abstracted from RRInterface.h
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

#ifndef LL_RRINTERFACEVERSION_H
#define LL_RRINTERFACEVERSION_H

// we need a switch to determine whether setsphere is activated (2.9.30 else 2.9.29) and since this header has no includes, it needs to be here
#define DISABLE_SETSPHERE

// Relocated to this file to avoid recompiling most of newview through a change to llagent.h (which includes RRinterface.h)

#ifdef DISABLE_SETSPHERE

#define RR_VERSION_NUM "2092901"
#define RR_VERSION "2.09.29.01"

#else //DISABLE_SETSPHERE

#define RR_VERSION_NUM "2093000"
#define RR_VERSION "2.09.30.00"

#endif // DISABLE_SETSPHERE

#endif // LL_RRINTERFACEVERSION_H
