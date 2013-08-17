/**
 * @file daeexport.h
 * @brief A system which allows saving in-world objects to Collada DAE format files for offline texturing or conversion to mesh etc.  Ported from Singularity by Jessica Wabbit.
 * @authors Latif Khalifa, Jessica Wabbit
 *
 * $LicenseInfo:firstyear=2013&license=LGPLV2.1$
 * Copyright (C) 2013 Latif Khalifa
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 */

#ifndef _DAEEXPORT_H_
#define _DAEEXPORT_H_

#include <dom/domElements.h>

class LLViewerObject;

namespace DAEExportUtil
{
    bool enable_export_object();
    void export_selection();
}

class DAESaver
{
	typedef std::vector<std::pair<LLViewerObject*,std::string> > obj_info_t;

public:
	obj_info_t mObjects;
	LLVector3 mOffset;
	DAESaver();
	void add(const LLViewerObject *prim, const std::string name);
	bool save_collada_dae(std::string filename);

private:
	void add_source(daeElement *mesh, const char *src_id, std::string params, const std::vector<F32> &vals);
	void add_polygons(daeElement *mesh, const char *geomID, const char *materialID, LLViewerObject *obj, int face_to_include);
};

#endif // _DAEEXPORT_H_
