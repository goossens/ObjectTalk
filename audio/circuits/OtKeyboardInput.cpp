//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
		pitchOutput = addOutputPin("Pitch", OtCircuitPinClass::Type::control);
		gateOutput = addOutputPin("Gate", OtCircuitPinClass::Type::control);
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		ImGui_PianoKeyboard(
			"Piano",
			ImVec2(itemWidth, height),
			&currentNote,
			36, 83,
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
					circuit->pitch = OtAudioUtilities::midiNoteToPitch(key);
					circuit->velocity = velocity;
				}

				if (msg == NoteOff) {
					circuit->keyPressed[key] = false;
					circuit->velocity = 0.0f;
				}

				return false;
			}, this);

			return false;
	}

	inline float getCustomRenderingWidth() override {
		return width;
	}

	inline float getCustomRenderingHeight() override {
		return height + ImGui::GetStyle().ItemSpacing.y;
	}

	// generate samples
	void execute() override {
		if (pitchOutput->isDestinationConnected()) {
			pitchOutput->setSamples(OtAudioUtilities::pitchToCv(pitch));
		}

		if (gateOutput->isDestinationConnected()) {
			gateOutput->setSamples(velocity == 0.0f ? 0.0f : 1.0f);
		}
	}

	static constexpr const char* circuitName = "Keyboard Input";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::input;
	static constexpr float width = 300.0f;
	static constexpr float height = 40.0f;

private:
	// properties
	int currentNote = 0;
	bool keyPressed[128];
	float pitch = 500.0f;
	float velocity = 0.0f;

	// work variables
	OtCircuitPin pitchOutput;
	OtCircuitPin gateOutput;
};

static OtCircuitFactoryRegister<OtKeyboardInputCircuit> registration;
