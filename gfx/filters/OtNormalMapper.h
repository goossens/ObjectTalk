//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>

#include "OtFilter.h"
#include "OtShaders.h"


//
//	OtNormalMapper
//

class OtNormalMapper : public OtFilter {
public:
	// set strength of normals
	inline void setStrength(float value) { normalStrength = value; }

	// determine if resulting normalmap includes height
	inline void includeHeight(bool value) { includeHeightFlag = value; }

	// configure the compute pass
	void configurePass(OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtNormalMapperComp, OtNormalMapperCompSize);
		}

		// set uniforms
		struct Uniforms {
			glm::vec2 texelSize;
			float normalStrength;
			int32_t includeHeightFlag;
		} uniforms {
			sourceTexelSize,
			normalStrength,
			static_cast<int32_t>(includeHeightFlag)
		};

		pass.addUniforms(&uniforms, sizeof(uniforms));
	}

private:
	// properties
	float normalStrength = 10.0f;
	bool includeHeightFlag = false;
};
