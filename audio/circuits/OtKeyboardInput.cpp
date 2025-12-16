//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ImGuiPiano.h"

#include "OtAudioUtilities.h"
#include "OtCircuitFactory.h"


//
//	OtKeyboardInputCircuit
//

class OtKeyboardInputCircuit : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		output = addOutputPin("Frequency", OtCircuitPinClass::Type::control);
	}

	// render custom fields
	inline void customRendering(float itemWidth) override {
		ImGui_PianoKeyboard(
			"Piano",
			ImVec2(itemWidth, height),
			&currentNote,
			24, 83,
			[](void* data, int msg, int key, float velocity) {
				OtKeyboardInputCircuit* circuit = (OtKeyboardInputCircuit*) data;

				if (key >= 128) {
					return false;
				}

				if (msg == NoteGetStatus) {
					return circuit->keyPressed[key];
				}

				if (msg == NoteOn) {
					circuit->keyPressed[key] = true;
					circuit->frequency = OtMidiNoteToFrequency(key);
					circuit->velocity = velocity;
				}

				if (msg == NoteOff) {
					circuit->keyPressed[key] = false;
					circuit->frequency = 0.0f;
					circuit->velocity = 0.0f;
				}

				return false;
			}, this);
	}

	inline float getCustomRenderingWidth() override {
		return width;
	}

	inline float getCustomRenderingHeight() override {
		return height;
	}

	// generate samples
	void execute([[maybe_unused]] size_t sampleRate, [[maybe_unused]] size_t samples) override {
		auto buffer = output->buffer;
		buffer->resize(samples);
		buffer->clear(OtFrequencyToCv(frequency));
	}

	static constexpr const char* circuitName = "Keyboard Input";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::input;
	static constexpr float width = 400.0f;
	static constexpr float height = 60.0f;

private:
	// properties
	OtCircuitPin output;

	int currentNote = 0;
	bool keyPressed[128];
	float frequency = 0.0f;
	float velocity = 0.0f;
};

static OtCircuitFactoryRegister<OtKeyboardInputCircuit> registration;
