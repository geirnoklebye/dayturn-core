/** 
 * @file lldrawpooltree.cpp
 * @brief LLDrawPoolTree class implementation
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

#include "llviewerprecompiledheaders.h"

#include "lldrawpooltree.h"

#include "lldrawable.h"
#include "llface.h"
#include "llsky.h"
#include "llvotree.h"
#include "pipeline.h"
#include "llviewercamera.h"
#include "llviewershadermgr.h"
#include "llrender.h"
#include "llviewercontrol.h"
#include "llviewerregion.h"
#include "llenvironment.h"

S32 LLDrawPoolTree::sDiffTex = 0;
static LLGLSLShader* shader = nullptr;

LLDrawPoolTree::LLDrawPoolTree(LLViewerTexture *texturep) :
	LLFacePool(POOL_TREE),
	mTexturep(texturep)
{
	mTexturep->setAddressMode(LLTexUnit::TAM_WRAP);
}

void LLDrawPoolTree::prerender()
{
	mShaderLevel = LLViewerShaderMgr::instance()->getShaderLevel(LLViewerShaderMgr::SHADER_OBJECT);
}

void LLDrawPoolTree::beginRenderPass(S32 pass)
{
	LL_RECORD_BLOCK_TIME(FTM_RENDER_TREES);
		
	if (LLPipeline::sUnderWaterRender)
	{
		shader = &gTreeWaterProgram;
	}
	else
	{
		shader = &gTreeProgram;
	}

	if (gPipeline.shadersLoaded())
	{
		shader->bind();
		shader->setMinimumAlpha(0.5f);
		gGL.diffuseColor4f(1,1,1,1);
	}
	else
	{
		gPipeline.enableLightsDynamic();
		gGL.flush();
	}
}

void LLDrawPoolTree::render(S32 pass)
{
	if (mDrawFace.empty())
	{
		return;
	}

	LLGLState test(GL_ALPHA_TEST, 0);

	gGL.getTexUnit(sDiffTex)->bindFast(mTexturep);
    gPipeline.touchTexture(mTexturep, 1024.f * 1024.f); // <=== keep Linden tree textures at full res

	for (std::vector<LLFace*>::iterator iter = mDrawFace.begin();
		 iter != mDrawFace.end(); iter++)
	{
		LLFace *face = *iter;
		LLVertexBuffer* buff = face->getVertexBuffer();

		if(buff)
		{
			LLMatrix4* model_matrix = &(face->getDrawable()->getRegion()->mRenderMatrix);

			if (model_matrix != gGLLastMatrix)
			{
				gGLLastMatrix = model_matrix;
				gGL.loadMatrix(gGLModelView);
				if (model_matrix)
				{
					llassert(gGL.getMatrixMode() == LLRender::MM_MODELVIEW);
					gGL.multMatrix((GLfloat*) model_matrix->mMatrix);
				}
				gPipeline.mMatrixOpCount++;
			}

			buff->setBufferFast(LLDrawPoolTree::VERTEX_DATA_MASK);
			buff->drawRangeFast(LLRender::TRIANGLES, 0, buff->getNumVerts()-1, buff->getNumIndices(), 0); 
		}
	}
}

void LLDrawPoolTree::endRenderPass(S32 pass)
{
	LL_RECORD_BLOCK_TIME(FTM_RENDER_TREES);
		
	if (gPipeline.canUseWindLightShadersOnObjects())
	{
		shader->unbind();
	}
	
	if (mShaderLevel <= 0)
	{
        gGL.flush();
	}
}

//============================================
// deferred implementation
//============================================
void LLDrawPoolTree::beginDeferredPass(S32 pass)
{
	LL_RECORD_BLOCK_TIME(FTM_RENDER_TREES);
		
	shader = &gDeferredTreeProgram;
	shader->bind();
	shader->setMinimumAlpha(0.5f);
}

void LLDrawPoolTree::renderDeferred(S32 pass)
{
	render(pass);
}

void LLDrawPoolTree::endDeferredPass(S32 pass)
{
	LL_RECORD_BLOCK_TIME(FTM_RENDER_TREES);
		
	shader->unbind();
}

//============================================
// shadow implementation
//============================================
void LLDrawPoolTree::beginShadowPass(S32 pass)
{
	glPolygonOffset(gSavedSettings.getF32("RenderDeferredTreeShadowOffset"),
					gSavedSettings.getF32("RenderDeferredTreeShadowBias"));

    LLEnvironment& environment = LLEnvironment::instance();

	gDeferredTreeShadowProgram.bind();
    gDeferredTreeShadowProgram.uniform1i(LLShaderMgr::SUN_UP_FACTOR, environment.getIsSunUp() ? 1 : 0);
	gDeferredTreeShadowProgram.setMinimumAlpha(0.5f);
}

void LLDrawPoolTree::renderShadow(S32 pass)
{
	render(pass);
}

void LLDrawPoolTree::endShadowPass(S32 pass)
{
	glPolygonOffset(gSavedSettings.getF32("RenderDeferredSpotShadowOffset"),
						gSavedSettings.getF32("RenderDeferredSpotShadowBias"));
	gDeferredTreeShadowProgram.unbind();
}

bool LLDrawPoolTree::verify() const
{
/*	BOOL ok = TRUE;

	if (!ok)
	{
		printDebugInfo();
	}
	return ok;*/

	return true;
}

LLViewerTexture *LLDrawPoolTree::getTexture()
{
	return mTexturep;
}

LLViewerTexture *LLDrawPoolTree::getDebugTexture()
{
	return mTexturep;
}


LLColor3 LLDrawPoolTree::getDebugColor() const
{
	return LLColor3(1.f, 0.f, 1.f);
}

