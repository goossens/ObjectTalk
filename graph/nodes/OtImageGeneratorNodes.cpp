//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtFbm.h"
#include "OtFrameBuffer.h"
#include "OtTexture.h"
#include "OtUi.h"

#include "OtGraphNode.h"
#include "OtImageGeneratorNodes.h"


//
//	OtNoiseMapGenerator
//

class OtNoiseMapGenerator : public OtGraphNodeClass {
public:
	// constructor
	inline OtNoiseMapGenerator() : OtGraphNodeClass(name, OtGraphNodeClass::generator) {}

	// configure node
	inline void configure() override {
		addInputPin("Width", width);
		addInputPin("Height", height);
		addInputPin("Frequency", frequency);
		addInputPin("Lacunarity", lacunarity);
		addInputPin("Amplitude", amplitude);
		addInputPin("Persistence", persistence);
		addInputPin("Octaves", octaves);
		addOutputPin("Image", texture);
	}

	// rendering custom fields
	void customRendering() override {
		ImGui::SetNextItemWidth(fieldWidth);
		auto old = serialize().dump();

		if (OtUiSelectorEnum("##noiseType", noiseType, OtFbm::noiseTypes, OtFbm::noiseTypeCount)) {
			oldState = old;
			newState = serialize().dump();
			needsEvaluating = true;
			needsSaving = true;
		}
	}

	float getCustomRenderingWidth() override {
		return fieldWidth;
	}

	float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize input
	void customSerialize(nlohmann::json* data, std::filesystem::path* basedir) override {
		(*data)["noiseType"] = noiseType;
	}

	void customDeserialize(nlohmann::json* data, std::filesystem::path* basedir) override {
		noiseType = data->value("noiseType", OtFbm::simplexNoiseType);
	}

	// running the noise generator
	void onExecute() override {
		// ensure framebuffer has right size
		framebuffer.update(width, height);

		// run the generator
		fbm.setFrequency(frequency);
		fbm.setLacunarity(lacunarity);
		fbm.setAmplitude(amplitude);
		fbm.setPersistence(persistence);
		fbm.setOctaves(octaves);
		fbm.setNoiseType(noiseType);
		fbm.render(framebuffer);

		// manage output (version numbers are used to detect changes down the line)
		texture = framebuffer.getColorTexture();
		texture.setVersion(version++);
	}

	static constexpr const char* name = "NoiseMap Generator";
	static constexpr float fieldWidth = 180.0f;

protected:
	int width = 256;
	int height = 256;
	float frequency = 10.0f;
	float lacunarity = 2.0f;
	float amplitude = 0.5f;
	float persistence = 0.5f;
	int octaves = 5;
	int noiseType = OtFbm::simplexNoiseType;

	OtFbm fbm;
	OtFrameBuffer framebuffer{OtFrameBuffer::rFloat32Texture};
	OtTexture texture;
	int version = 1;
};


//
//	OtImageGeneratorNodesRegister
//

#define REGISTER(CLASS) \
	graph.registerNodeType<CLASS>("Image Generators", CLASS::name)

void OtImageGeneratorNodesRegister(OtGraph& graph) {
	REGISTER(OtNoiseMapGenerator);
}
