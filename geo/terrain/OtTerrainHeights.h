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

	// access properties
	inline void setMapSize(int value) { heightMapSize = value; }
	inline void setMapDensity(float value) { heightMapDensity = value; }
	inline void setNormalStrength(float value) { normalStrength = value; }

	inline int getMapSize() { return heightMapSize; }
	inline float getMapDensity() { return heightMapDensity; }
	inline float getNormalStrength() { return normalStrength; }

	inline void setNoiseFrequency(int value) { noise.frequency = value; }
	inline void setNoiseLacunarity(int value) { noise.lacunarity = value; }
	inline void setNoiseAmplitude(float value) { noise.amplitude = value; }
	inline void setNoisePersistence(float value) { noise.persistence = value; }
	inline void setNoiseOctaves(int value) { noise.octaves = value; }

	inline int getNoiseFrequency() { return noise.frequency; }
	inline int getNoiseLacunarity() { return noise.lacunarity; }
	inline float getNoiseAmplitude() { return noise.amplitude; }
	inline float getNoisePersistence() { return noise.persistence; }
	inline int getNoiseOctaves() { return noise.octaves; }

	inline void setErosionRounding(glm::vec4 value) { erosion.rounding = value; }
	inline void setErosionOnset(glm::vec4 value) { erosion.onset = value; }
	inline void setErosionAssumedSlope(glm::vec2 value) { erosion.assumedSlope = value; }
	inline void setErosionScale(float value) { erosion.scale = value; }
	inline void setErosionStrength(float value) { erosion.strength = value; }
	inline void setErosionGullyWeight(float value) { erosion.gullyWeight = value; }
	inline void setErosionDetail(float value) { erosion.detail = value; }
	inline void setErosionCellScale(float value) { erosion.cellScale = value; }
	inline void setErosionGain(float value) { erosion.gain = value; }
	inline void setErosionLacunarity(float value) { erosion.lacunarity = value; }
	inline void setErosionNormalization(float value) { erosion.normalization = value; }
	inline void setErosionOctaves(int value) { erosion.octaves = value; }

	inline glm::vec4 getErosionRounding() { return erosion.rounding; }
	inline glm::vec4 getErosionOnset() { return erosion.onset; }
	inline glm::vec2 getErosionAssumedSlope() { return erosion.assumedSlope; }
	inline float getErosionScale() { return erosion.scale; }
	inline float getErosionStrength() { return erosion.strength; }
	inline float getErosionGullyWeight() { return erosion.gullyWeight; }
	inline float getErosionDetail() { return erosion.detail; }
	inline float getErosionCellScale() { return erosion.cellScale; }
	inline float getErosionGain() { return erosion.gain; }
	inline float getErosionLacunarity() { return erosion.lacunarity; }
	inline float getErosionNormalization() { return erosion.normalization; }
	inline int getErosionOctaves() { return erosion.octaves; }

	// rendering tools
	bool textureDirty = true;
	bool noiseDirty = true;
	bool erosionDirty = true;
	bool normalsDirty = true;

	// the maps on the GPU
	OtTexture noiseMap;
	OtTexture erodedMap;
	OtTexture normalMap;

	// update the heightmap
	bool needsUpdate() { return textureDirty | noiseDirty | erosionDirty | normalsDirty; }
	void update();

private:
	friend class OtSceneRenderEntitiesPass;

	// properties
	int heightMapSize = 256;
	float heightMapDensity = 1.0f;
	float normalStrength = 10.0f;

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

	struct Erosion : public OtFilter {
	public:
		// configure the compute pass
		void configurePass(OtComputePass& pass) override {
			// initialize pipeline (if required)
			if (!pipeline.isValid()) {
				pipeline.setShader(OtTerrainErosionComp, OtTerrainErosionCompSize);
			}

			// set uniforms
			struct Uniforms {
				glm::vec4 rounding;
				glm::vec4 onset;
				glm::vec2 assumedSlope;
				glm::vec2 texelSize;
				float scale;
				float strength;
				float gullyWeight;
				float detail;
				float cellScale;
				float gain;
				float lacunarity;
				float normalization;
				int32_t octaves;
			} uniforms {
				rounding,
				onset,
				assumedSlope,
				sourceTexelSize,
				scale,
				strength,
				gullyWeight,
				detail,
				cellScale,
				gain,
				lacunarity,
				normalization,
				octaves
			};

			pass.addUniforms(&uniforms, sizeof(uniforms));
		}

		// properties
		glm::vec4 rounding{0.1f, 0.0f, 0.1f, 2.0f};
		glm::vec4 onset{1.25f, 1.25f, 2.8f, 1.5f};
		glm::vec2 assumedSlope{0.7f, 1.0f};
		float scale = 0.15f;
		float strength = 0.22f;
		float gullyWeight = 0.5f;
		float detail = 1.5f;
		float cellScale = 0.7f;
		float gain = 0.5f;
		float lacunarity = 2.0f;
		float normalization = 0.5f;
		int octaves = 5;
	} erosion;

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
