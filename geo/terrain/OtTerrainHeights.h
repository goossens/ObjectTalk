//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "nlohmann/json_fwd.hpp"

#include "OtFilter.h"
#include "OtGenerator.h"
#include "OtShaders.h"
#include "OtTexture.h"


//
//	OtTerrainHeights
//

class OtTerrainHeights {
public:
	// UI to change properties
	bool renderUI();

	// (de)serialize material
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json& data, std::string* basedir);

	// properties
	int heightMapSize = 256;
	float normalStrength = 10.0f;

	// rendering tools
	bool dirty = true;

	// the maps on the GPU
	OtTexture heightmap;
	OtTexture normalmap;

	// update the heightmap
	void update();

private:
	// generators/filters
	struct Noise : public OtGenerator {
		// configure the compute pass
		void configurePass(OtComputePass& pass) override {
			// initialize pipeline (if required)
			if (!pipeline.isValid()) {
				pipeline.setShader(OtTerrainNoiseComp, OtTerrainNoiseCompSize);
			}

			// set uniforms
			struct Uniforms {
				int32_t frequency;
				int32_t lacunarity;
				float amplitude;
				float persistence;
				int32_t octaves;
			} uniforms{
				static_cast<int32_t>(frequency),
				static_cast<int32_t>(lacunarity),
				amplitude,
				persistence,
				static_cast<int32_t>(octaves)
			};

			pass.addUniforms(&uniforms, sizeof(uniforms));
		}

		// properties
		int frequency = 10;
		int lacunarity = 2;
		float amplitude = 0.5f;
		float persistence = 0.5f;
		int octaves = 5;
	} noise;

	struct Normals : public OtFilter {
	public:
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
			} uniforms {
				sourceTexelSize,
				normalStrength
			};

			pass.addUniforms(&uniforms, sizeof(uniforms));
		}

		// properties
		float normalStrength = 10.0f;
	} normals;
};
