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
#include "OtPass.h"

#include "OtSceneRenderPass.h"


//
//	OtBackgroundPass
//

class OtBackgroundPass : public OtSceneRenderPass {
public:
	// constructor
	OtBackgroundPass(OtFrameBuffer& fb) : framebuffer(fb) {}

	// render the pass
	void render(OtSceneRendererContext& ctx) {
		// determine background color
		glm::vec3 backgroundColor{0.0f};

		for (auto&& [entity, component] : ctx.scene->view<OtBackgroundComponent>().each()) {
			backgroundColor = component.color;
		}

		// setup pass
		OtPass pass;
		pass.setRectangle(0, 0, ctx.camera.width, ctx.camera.height);
		pass.setFrameBuffer(framebuffer);
		pass.setClear(true, true, glm::vec4(backgroundColor, 1.0f));
		pass.touch();
	}

private:
	// properties
	OtFrameBuffer& framebuffer;
};
