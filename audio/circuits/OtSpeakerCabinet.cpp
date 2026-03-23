//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "FFTConvolver.h"
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtAsset.h"

#include "OtAudioSettings.h"
#include "OtAudioUi.h"
#include "OtCircuitFactory.h"

#include "OtCabinetIR.h"


//
//	OtSpeakerCabinetCircuit
//

class OtSpeakerCabinetCircuit : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		audioInput = this->addInputPin("Input", OtCircuitPinClass::Type::mono);
		audioOutput = this->addOutputPin("Output", OtCircuitPinClass::Type::mono);
		setCabinetIR();
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		static constexpr const char* cabinetTypes[] = {
			"1x8",
			"1x12",
			"1x15",
			"2x10",
			"4x10",
			"2x12",
			"4x12",
			"AC30",
			"Bassman",
			"HighGain",
			"Marshall",
			"Mesa",
			"Princeton",
			"Twin"
		};

		static constexpr size_t cabinetTypeCount = sizeof(cabinetTypes) / sizeof(*cabinetTypes);

		ImGui::SetNextItemWidth(itemWidth - (ImGui::CalcTextSize("X").x * 7.0f + ImGui::GetStyle().ItemInnerSpacing.x));
		auto changed = OtUi::selectorEnum("Cabinet", &cabinet, cabinetTypes, cabinetTypeCount);

		if (changed) {
			setCabinetIR();
		}

		return changed;
	}


	inline float getCustomRenderingWidth() override {
		return 180.0f;
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["cabinet"] = cabinet;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		cabinet = data->value("cabinet", OtCabinetType::cabMarshall);
		setCabinetIR();
	}

	// run for one buffer
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			float output[OtAudioSettings::bufferSize];

			if (audioInput->isSourceConnected()) {
				float input[OtAudioSettings::bufferSize];
				audioInput->getSamples(input);
				convolver.process(input, output, OtAudioSettings::bufferSize);

			} else {
				std::fill(output, output + OtAudioSettings::bufferSize, 0.0f);
			}

			audioOutput->setSamples(output);
		}
	}

	static constexpr const char* circuitName = "Speaker Cabinet";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::amp;

private:
	// properties
	OtCabinetType cabinet = OtCabinetType::cabMarshall;
	OtCabinetIR* ir;
	fftconvolver::FFTConvolver convolver;

	// work variables
	OtCircuitPin audioInput;
	OtCircuitPin audioOutput;

	// specify a new cabinet IR
	void setCabinetIR() {
		ir = OtCabinetIRGet(cabinet);
		convolver.init(OtAudioSettings::bufferSize, ir->data, ir->size);
	}
};

static OtCircuitFactoryRegister<OtSpeakerCabinetCircuit> registration;
