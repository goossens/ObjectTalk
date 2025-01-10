//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

#include "OtNodesFactory.h"


//
//	OtNoiseMapGeneratorNode
//

class OtNoiseMapGeneratorNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Width", width);
		addInputPin("Height", height);
		addInputPin("Frequency", frequency);
		addInputPin("Lacunarity", lacunarity);
		addInputPin("Amplitude", amplitude);
		addInputPin("Persistence", persistence);
		addInputPin("Octaves", octaves);
		addOutputPin("Texture", texture);
	}

	// render custom fields
	void customRendering(float width) override {
		ImGui::SetNextItemWidth(width);
		auto old = serialize().dump();

		if (OtUi::selectorEnum("##noiseType", &noiseType, OtFbm::noiseTypes, OtFbm::noiseTypeCount)) {
			oldState = old;
			newState = serialize().dump();
			needsEvaluating = true;
			needsSaving = true;
		}
	}

	float getCustomRenderingWidth() override {
		return 180.0f;
	}

	float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize node
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["noiseType"] = noiseType;
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		noiseType = data->value("noiseType", OtFbm::NoiseType::simplex);
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

	static constexpr const char* nodeName = "NoiseMap Generator";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::generate;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	int width = 256;
	int height = 256;
	float frequency = 10.0f;
	float lacunarity = 2.0f;
	float amplitude = 0.5f;
	float persistence = 0.5f;
	int octaves = 5;
	OtFbm::NoiseType noiseType = OtFbm::NoiseType::simplex;

	OtFbm fbm;
	OtFrameBuffer framebuffer{OtTexture::rFloat32Texture};
	OtTexture texture;
	int version = 1;
};

static OtNodesFactoryRegister<OtNoiseMapGeneratorNode> type;
