//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::render
//

int OtSceneRenderer::render(OtScene2 scene, OtCamera2 c, int w, int h) {
	// save information
	camera = c;
	width = w;
	height = h;

	// update buffers
	gbuffer.update(width, height);
	composite.update(width, height);

	// render all passes
	renderGeometryPass(scene);
	renderBackgroundPass(scene);
	renderLightingPass(scene);

	return composite.getColorTextureIndex();
}
