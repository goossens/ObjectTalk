//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFrameBuffer.h"
#include "OtShaderProgram.h"

#include "OtSceneRenderEntitiesPass.h"


//
//	OtForwardPass
//

class OtForwardPass : public OtSceneRenderEntitiesPass {
public:
	// constructor
	OtForwardPass(OtFrameBuffer& fb) : framebuffer(fb) {}

	// render the pass
	void render(OtSceneRendererContext& ctx);

protected:
	// methods that must be overriden by subclasses (when required)
	inline bool isRenderingOpaque() override { return false; };
	inline bool isRenderingTransparent() override { return true; };

	void renderTransparentGeometry(OtSceneRendererContext& ctx, OtGeometryRenderData& grd) override;

private:
	// properties
	OtFrameBuffer& framebuffer;
	OtShaderProgram transparentProgram{"OtForwardVS", "OtForwardPbrFS"};
	OtShaderProgram instancedTransparentProgram{"OtForwardInstancingVS", "OtForwardPbrFS"};
};
