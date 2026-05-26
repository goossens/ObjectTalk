//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtCircuitFactory.h"


//
//	OtVuMeter
//

class OtVuMeter : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		input = addInputPin("Input", OtCircuitPinClass::Type::stereo);
	}


	// render custom fields
	inline bool customRendering([[maybe_unused]] float itemWidth) override {
		OtAudioUi::dbfsRenderV(leftState);
		ImGui::SameLine();
		OtAudioUi::dbfsRenderV(rightState);
		return false;
	}

	inline float getCustomRenderingWidth() override {
		return ImGui::GetFrameHeight() * 2.0f + ImGui::GetStyle().ItemSpacing.x;
	}

	inline float getCustomRenderingHeight() override {
		return OtAudioUi::dbfsSizeV();
	}

	// process samples
	inline void execute() override {
		if (input->isSourceConnected()) {
			float buffer[OtAudioSettings::bufferSize * 2];
			input->getSamples(buffer);

			OtAudioUi::dbfsUpdate(leftState, buffer, OtAudioSettings::bufferSize);
			OtAudioUi::dbfsUpdate(rightState, buffer + OtAudioSettings::bufferSize, OtAudioSettings::bufferSize);

		} else {
			OtAudioUi::dbfsReset(leftState);
			OtAudioUi::dbfsReset(rightState);
		}
	}

	static constexpr const char* circuitName = "Vu Meter";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::probe;

private:
	// properties
	OtCircuitPin input;

	OtAudioUi::dbfsState leftState;
	OtAudioUi::dbfsState rightState;
};

static OtCircuitFactoryRegister<OtVuMeter> registration;
