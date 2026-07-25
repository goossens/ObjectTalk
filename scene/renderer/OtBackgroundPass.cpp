//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp" // IWYU pragma: keep

#include "OtRenderPass.h"

#include "OtBackgroundPass.h"


//
//	OtBackgroundPass::render
//

void OtBackgroundPass::render(OtSceneRendererContext& ctx) {
	// determine background color
	glm::vec3 backgroundColor{0.0f};

	for (auto&& [entity, component] : ctx.scene->view<OtBackgroundComponent>().each()) {
		backgroundColor = component.color;
	}

	OtRenderPass pass;
	pass.setClearColor(true, glm::vec4(backgroundColor, 1.0f));
	pass.setClearDepth(true);
	pass.start(framebuffer);
	pass.end();
}
