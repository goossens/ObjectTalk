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
	// access camera angles
	inline void setAzimuth(float a) { azimuth = a; }
	inline void setElivation(float e) { elevation = e; }
	inline float getAzimuth() { return azimuth; }
	inline float getElevation() { return elevation; }

	// render to a specified framebuffer
	void render(OtFrameBuffer& framebuffer, OtMesh& mesh);

private:
	// properties
	float azimuth = 180.0f;
	float elevation = 20.0f;
	glm::vec3 meshColor{1.0f, 0.85f, 0.0f};
	glm::vec3 lightColor{1.0f};
	bool wireframe = false;

	// rendering variables
	OtPass pass;
	OtUniformVec4 uniform = OtUniformVec4("u_preview", 4);
	OtShaderProgram program = OtShaderProgram("OtPreviewVS", "OtPreviewFS");
};
