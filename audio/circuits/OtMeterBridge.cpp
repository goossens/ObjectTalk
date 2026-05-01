//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>
#include <mutex>
#include <vector>

#include "OtAudioSettings.h"
#include "OtAudioUi.h"
#include "OtAudioUtilities.h"
#include "OtCircuitFactory.h"


//
//	OtMeterBridge
//

class OtMeterBridge : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		input1 = addInputPin("Input 1", OtCircuitPinClass::Type::mono);
		input2 = addInputPin("Input 2", OtCircuitPinClass::Type::mono);
		input3 = addInputPin("Input 3", OtCircuitPinClass::Type::mono);
		input4 = addInputPin("Input 4", OtCircuitPinClass::Type::mono);
		input5 = addInputPin("Input 5", OtCircuitPinClass::Type::mono);
		input6 = addInputPin("Input 6", OtCircuitPinClass::Type::mono);
		input7 = addInputPin("Input 7", OtCircuitPinClass::Type::mono);
		input8 = addInputPin("Input 8", OtCircuitPinClass::Type::mono);
	}

	// render custom fields
	inline bool customRendering([[maybe_unused]] float itemWidth) override {
		meter1.render(); ImGui::SameLine();
		meter2.render(); ImGui::SameLine();
		meter3.render(); ImGui::SameLine();
		meter4.render(); ImGui::SameLine();
		meter5.render(); ImGui::SameLine();
		meter6.render(); ImGui::SameLine();
		meter7.render(); ImGui::SameLine();
		meter8.render();
		return false;
	}

	inline float getCustomRenderingWidth() override {
		return OtAudioUi::dbfsSize().x * 8.0f + ImGui::GetStyle().ItemSpacing.x * 7.0f;
	}

	inline float getCustomRenderingHeight() override {
		return OtAudioUi::dbfsSize().y;
	}

	// process samples
	void execute() override {
		if (input1->isSourceConnected()) { meter1.update(input1); } else { meter1.reset(); }
		if (input2->isSourceConnected()) { meter2.update(input2); } else { meter2.reset(); }
		if (input3->isSourceConnected()) { meter3.update(input3); } else { meter3.reset(); }
		if (input4->isSourceConnected()) { meter4.update(input4); } else { meter4.reset(); }
		if (input5->isSourceConnected()) { meter5.update(input5); } else { meter5.reset(); }
		if (input6->isSourceConnected()) { meter6.update(input6); } else { meter6.reset(); }
		if (input7->isSourceConnected()) { meter7.update(input7); } else { meter7.reset(); }
		if (input8->isSourceConnected()) { meter8.update(input8); } else { meter8.reset(); }
	};

	static constexpr const char* circuitName = "Meter Bridge";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::probe;

private:
	// properties
	OtCircuitPin input1;
	OtCircuitPin input2;
	OtCircuitPin input3;
	OtCircuitPin input4;
	OtCircuitPin input5;
	OtCircuitPin input6;
	OtCircuitPin input7;
	OtCircuitPin input8;

	// actual meters
	struct Meter {
		inline void reset() {
			std::lock_guard<std::mutex> guard(mutex);

			if (history.size()) {
				decibels = -60.0f;
				history.clear();
			}
		}

		inline void update(OtCircuitPin& pin) {
			auto sumOfSquares = 0.0f;

			for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
				auto sample = pin->getSample(i);
				sumOfSquares += sample * sample;
			}

			auto rms = std::sqrt(sumOfSquares / OtAudioSettings::bufferSize);
			decibels = OtAudioUtilities::linearToDbfs(rms);

			std::lock_guard<std::mutex> guard(mutex);

			if (history.size() < maxHistory) {
				history.emplace_back(decibels);

			} else {
				history[historyIndex] = decibels;
				historyIndex = (historyIndex + 1) % maxHistory;
			}
		}

		inline void render() {
			OtAudioUi::dbfs(decibels);

			if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
				float average = 0.0f;
				float minimum = 0.0f;
				float maximum = std::numeric_limits<float>::lowest();

				{
					std::lock_guard<std::mutex> guard(mutex);

					for (auto sample : history) {
						average += sample;
						minimum = std::min(minimum, sample);
						maximum = std::max(maximum, sample);
					}

					average /= static_cast<float>(history.size());
				}

				ImGui::SetTooltip(
					"Cur: %06.2f dBFS\nAvg: %06.2f dBFS\nMin: %06.2f dBFS\nMax: %06.2f dBFS",
					decibels,
					average,
					minimum,
					maximum);
			}
		}

		float decibels = -60.0;

		static constexpr float maxHistoryInSeconds = 1.5f;
		static constexpr size_t maxHistory = static_cast<size_t>(maxHistoryInSeconds * OtAudioSettings::sampleRate / OtAudioSettings::bufferSize);

		std::vector<float> history;
		size_t historyIndex;
		std::mutex mutex;
	};

	Meter meter1;
	Meter meter2;
	Meter meter3;
	Meter meter4;
	Meter meter5;
	Meter meter6;
	Meter meter7;
	Meter meter8;
};

static OtCircuitFactoryRegister<OtMeterBridge> registration;
