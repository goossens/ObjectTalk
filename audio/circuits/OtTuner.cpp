//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <mutex>

#include "imgui.h"

#include "OtUi.h"

#include "OtAudioSettings.h"
#include "OtAudioUtilities.h"
#include "OtCircuitFactory.h"
#include "OtCircularBuffer.h"
#include "OtPitchYin.h"


//
//	OtTuner
//

class OtTuner : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		input = addInputPin("Input", OtCircuitPinClass::Type::mono);
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		if (input->isSourceConnected()) {
			{
				// quickly copy signal to separate buffer
				// to avoid race conditions and to not hold up audio thread
				std::lock_guard<std::mutex> guard(mutex);
				std::copy(data.begin(), data.end(), inputBuffer);
			}

			// determine dominant frequency and translate to closest note
			float frequency = pitch.calculatePitch(inputBuffer);
			auto midiNote = OtAudioUtilities::frequencyToClosestMidiNote(frequency);
			auto noteName = OtAudioUtilities::midiNoteToText(midiNote);
			auto noteFrequency = OtAudioUtilities::midiNoteToFrequency(midiNote);
			auto difference = OtAudioUtilities::differenceInCents(noteFrequency, frequency);

			// display results
			ImGui::PushItemWidth(itemWidth - (ImGui::CalcTextSize("X").x * 18.0f + ImGui::GetStyle().ItemInnerSpacing.x));
			OtUi::readonlyFloat("Dominant Frequency", frequency, "{:.1f}");
			OtUi::readonlyText("Closest Note", noteName);
			OtUi::readonlyFloat("Difference (cents)", difference, "{:.1f}");
			ImGui::PopItemWidth();

		} else {
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted("No input signal");
		}

		return false;
	}

	inline float getCustomRenderingWidth() override {
		return customW;
	}

	inline float getCustomRenderingHeight() override {
		return customH + ImGui::GetStyle().ItemSpacing.y;
	}

	// process samples
	inline void execute() override {
		if (input->isSourceConnected()) {
			customW = width;
			customH = ImGui::GetFrameHeight() * 2.0f;

			// add input signal to data buffer
			float buffer[OtAudioSettings::bufferSize];
			input->getSamples(buffer);

			std::lock_guard<std::mutex> guard(mutex);
			data.insert(buffer, OtAudioSettings::bufferSize);

		} else {
			customW = width;
			customH = ImGui::GetFrameHeight();
		}

		needsSizing = true;
	}

	static constexpr const char* circuitName = "Tuner";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::probe;
	static constexpr float width = 250.0f;
	static constexpr size_t N = 4096;

private:
	// properties
	OtCircuitPin input;

	float customW;
	float customH;

	OtPitchYin<float, N> pitch;

	std::mutex mutex;
	OtCircularBuffer<float, N> data;
	float inputBuffer[N];
};

static OtCircuitFactoryRegister<OtTuner> registration;
