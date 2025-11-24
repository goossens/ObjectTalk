//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtFrameBuffer.h"
#include "OtMesh.h"
#include "OtRenderPipeline.h"


//
//	OtMeshPreview
//

class OtMeshPreview {
public:
	// constructor
	OtMeshPreview();

	// rendering context
	class Context {
	public:
		float azimuth = 0.0f;
		float elevation = 0.0f;
		float zoom = 0.95f;
		glm::vec3 meshColor{1.0f, 0.85f, 0.0f};
		glm::vec3 lightColor{1.0f};
		bool wireframe = false;
	};

	// render mesh as an ImGui widget
	void render(int width, int height, OtMesh& mesh, Context& context);

private:
	// constants
	static constexpr float maxRotationPerSecond = 20.0f;
	static constexpr float maxZoomPerSecond = 0.2f;

	// rendering variables
	OtFrameBuffer framebuffer{OtTexture::Format::rgba8, OtTexture::Format::d32};
	OtRenderPipeline fillPipeline;
	OtRenderPipeline linePipeline;
};
