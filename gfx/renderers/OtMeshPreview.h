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
#include "OtPass.h"
#include "OtShaderProgram.h"
#include "OtUniformVec4.h"


//
//	OtMeshPreview
//

class OtMeshPreview {
public:
	// set preview size
	void setSize(int width, int height);

	// access camera angles
	inline void setAzimuth(float a) { azimuth = a; }
	inline void setElivation(float e) { elevation = e; }
	inline float getAzimuth() { return azimuth; }
	inline float getElevation() { return elevation; }

	// render mesh as an ImGui widget
	void render(OtMesh& mesh);

private:
	// properties
	int width = 0;
	int height = 0;
	float azimuth = 0.0f;
	float elevation = 0.0f;
	glm::vec3 meshColor{1.0f, 0.85f, 0.0f};
	glm::vec3 lightColor{1.0f};
	bool wireframe = false;

	// constants
	static constexpr float maxRotationPerSecond = 20.0f;

	// rendering variables
	OtPass pass;
	OtUniformVec4 uniform = OtUniformVec4("u_preview", 4);
	OtShaderProgram program = OtShaderProgram("OtPreviewVS", "OtPreviewFS");
	OtFrameBuffer framebuffer{OtTexture::rgba8Texture, OtTexture::d24s8Texture};
};
