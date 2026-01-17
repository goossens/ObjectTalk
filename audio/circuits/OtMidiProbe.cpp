//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <array>

#include "imgui.h"

#include "ImGuiPiano.h"

#include "OtCircuitFactory.h"


//
//	OtMidiProbe
//

class OtMidiProbe : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		input = addInputPin("Input", OtCircuitPinClass::Type::midi);
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		if (input->isSourceConnected()) {
			ImGui::SetNextItemWidth(itemWidth);

			ImGui_PianoKeyboard(
				"Piano",
				ImVec2(itemWidth, height),
				&currentNote,
				36, 83,
				[](void* data, int msg, int key, [[maybe_unused]] float velocity) {
					OtMidiProbe* probe = (OtMidiProbe*) data;

					if (key >= 128) {
						return false;
					}

					if (msg == NoteGetStatus) {
						return probe->keyPressed[key];
					}

					if (msg == NoteOn) {
						probe->keyPressed[key] = true;
					}

					if (msg == NoteOff) {
						probe->keyPressed[key] = false;
					}

					return false;
				}, this);

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

	// process input
	inline void execute() override {
		if (input->isSourceConnected()) {
			customW = width;
			customH = height;

			auto& messages = input->getMidiInputBuffer();

			for (auto& message : messages) {
				if (message->isNoteOn()) {
					keyPressed[message->getKeyNumber()] = true;

				} else if (message->isNoteOff()) {

					keyPressed[message->getKeyNumber()] = false;

				} else if (message->isAllNotesOff()) {
					std::fill(keyPressed.begin(), keyPressed.end(), false);
				}
			}

		} else {
			customW = width;
			customH = ImGui::GetFrameHeight();
		}

		needsSizing = true;
	}

	static constexpr const char* circuitName = "MIDI Probe";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::probe;
	static constexpr float width = 300.0f;
	static constexpr float height = 40.0f;

private:
	// properties
	OtCircuitPin input;

	// work variables
	std::array<bool, 128> keyPressed = {false};
	int currentNote = 0;

	float customW;
	float customH;
};

static OtCircuitFactoryRegister<OtMidiProbe> registration;
