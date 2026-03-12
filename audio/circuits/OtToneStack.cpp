//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtAsset.h"

#include "OtAudioSettings.h"
#include "OtAudioUi.h"
#include "OtCircuitFactory.h"

#include "OtToneStackAmpeg.h"
#include "OtToneStackBogner.h"
#include "OtToneStackFender.h"
#include "OtToneStackIbanez.h"
#include "OtToneStackMarshall.h"
#include "OtToneStackMesa.h"
#include "OtToneStackPeavey.h"
#include "OtToneStackRoland.h"
#include "OtToneStackSoldano.h"
#include "OtToneStackVox.h"


//
//	OtToneStackCircuit
//

class OtToneStackCircuit : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		audioInput = this->addInputPin("Input", OtCircuitPinClass::Type::mono);
		audioOutput = this->addOutputPin("Output", OtCircuitPinClass::Type::mono);
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		// tonestack types
		static constexpr const char* tonestackTypes[] = {
			"Ampeg VL 501",
			"Bogner Tripple",
			"Fender Twin",
			"Ibanez GX20",
			"Marshall JCM800",
			"Mesa Boogie Mark",
			"Peavey C20",
			"Roland Cube 60",
			"Soldano SLO-100",
			"Vox AC30"
		};

		static constexpr size_t tonestackTypeCount = sizeof(tonestackTypes) / sizeof(*tonestackTypes);

		ImGui::SetNextItemWidth(itemWidth - (ImGui::CalcTextSize("X").x * 4.0f + ImGui::GetStyle().ItemInnerSpacing.x));
		auto changed = OtUi::selectorEnum("Type", &tonestackType, tonestackTypes, tonestackTypeCount);

		changed |= OtAudioUi::knob("Bass", &bass, 0.0f, 1.0f, "%.2f"); ImGui::SameLine();
		changed |= OtAudioUi::knob("Middle", &middle, 0.0f, 1.0f, "%.2f"); ImGui::SameLine();
		changed |= OtAudioUi::knob("Treble", &treble, 0.0f, 1.0f, "%.2f");
		return changed;
	}


	inline float getCustomRenderingWidth() override {
		return OtAudioUi::knobWidth(3);
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing() + OtAudioUi::knobHeight();
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["tonestackType"] = tonestackType;
		(*data)["bass"] = bass;
		(*data)["middle"] = middle;
		(*data)["treble"] = treble;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		tonestackType = data->value("tonestackType", Type::marshall);
		bass = data->value("bass", 0.5f);
		middle = data->value("middle", 0.5f);
		treble = data->value("treble", 0.5f);
	}

	// generate output by evaluating MIDI messages
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			float output[OtAudioSettings::bufferSize];

			if (audioInput->isSourceConnected()) {
				float input[OtAudioSettings::bufferSize];
				audioInput->getSamples(input);

				auto in = input;
				auto out = output;

				switch (tonestackType) {
					case Type::ampeg:
						ampeg.setBass(bass);
						ampeg.setMiddle(middle);
						ampeg.setTreble(treble);
						ampeg.compute(OtAudioSettings::bufferSize, &in, &out);
						break;

					case Type::bogner:
						bogner.setBass(bass);
						bogner.setMiddle(middle);
						bogner.setTreble(treble);
						bogner.compute(OtAudioSettings::bufferSize, &in, &out);
						break;

					case Type::fender:
						fender.setBass(bass);
						fender.setMiddle(middle);
						fender.setTreble(treble);
						fender.compute(OtAudioSettings::bufferSize, &in, &out);
						break;

					case Type::ibanez:
						ibanez.setBass(bass);
						ibanez.setMiddle(middle);
						ibanez.setTreble(treble);
						ibanez.compute(OtAudioSettings::bufferSize, &in, &out);
						break;

					case Type::marshall:
						marshall.setBass(bass);
						marshall.setMiddle(middle);
						marshall.setTreble(treble);
						marshall.compute(OtAudioSettings::bufferSize, &in, &out);
						break;

					case Type::mesa:
						mesa.setBass(bass);
						mesa.setMiddle(middle);
						mesa.setTreble(treble);
						mesa.compute(OtAudioSettings::bufferSize, &in, &out);
						break;

					case Type::peavey:
						peavey.setBass(bass);
						peavey.setMiddle(middle);
						peavey.setTreble(treble);
						peavey.compute(OtAudioSettings::bufferSize, &in, &out);
						break;

					case Type::roland:
						roland.setBass(bass);
						roland.setMiddle(middle);
						roland.setTreble(treble);
						roland.compute(OtAudioSettings::bufferSize, &in, &out);
						break;

					case Type::soldano:
						soldano.setBass(bass);
						soldano.setMiddle(middle);
						soldano.setTreble(treble);
						soldano.compute(OtAudioSettings::bufferSize, &in, &out);
						break;

					case Type::vox:
						vox.setBass(bass);
						vox.setMiddle(middle);
						vox.setTreble(treble);
						vox.compute(OtAudioSettings::bufferSize, &in, &out);
						break;
				}

			} else {
				std::fill(output, output + OtAudioSettings::bufferSize, 0.0f);
			}

			audioOutput->setSamples(output);
		}
	}

	static constexpr const char* circuitName = "Tone Stack";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::equalizer;

private:
	// properties
	enum class Type {
		ampeg,
		bogner,
		fender,
		ibanez,
		marshall,
		mesa,
		peavey,
		roland,
		soldano,
		vox
	};

	Type tonestackType = Type::marshall;
	float bass = 0.5f;
	float middle = 0.5f;
	float treble = 0.5f;

	// work variables
	OtCircuitPin audioInput;
	OtCircuitPin audioOutput;

	OtToneStackAmpeg ampeg;
	OtToneStackBogner bogner;
	OtToneStackFender fender;
	OtToneStackIbanez ibanez;
	OtToneStackMarshall marshall;
	OtToneStackMesa mesa;
	OtToneStackPeavey peavey;
	OtToneStackRoland roland;
	OtToneStackSoldano soldano;
	OtToneStackVox vox;
};

static OtCircuitFactoryRegister<OtToneStackCircuit> registration;
