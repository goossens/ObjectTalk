//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <mutex>

#include "imgui.h"
#include "implot.h"

#include "OtCircuitFactory.h"
#include "OtCircularBuffer.h"


//
//	OtScope
//

class OtScope : public OtCircuitClass {
public:
	// configure node
	inline void configure() override {
		input = addInputPin("Input", OtCircuitPinClass::Type::mono);
	}

	// process frame
	inline void execute() override {
		if (input->isSourceConnected()) {
			customW = width;
			customH = height;

			// add input signal to data buffer
			std::lock_guard<std::mutex> guard(mutex);
			auto signal = input->getSignalBuffer();
			data.insert(input->getSignalBuffer()->data(), signal->getSampleCount());

		} else {
			customW = width;
			customH = ImGui::GetFrameHeightWithSpacing();

		}

		needsSizing = true;
	}

	// render custom fields
	inline void customRendering(float itemWidth) override {
		if (input->isSourceConnected()) {
			// find first rising zero crossing
			std::lock_guard<std::mutex> guard(mutex);
			size_t crossing = 0;

			for (size_t i = 1; crossing == 0 && i < data.size(); i++) {
				if (data[i - 1] <= 0.0f && data[i] >= 0.0f) {
					crossing = i;
				}
			}

			if (ImPlot::BeginPlot("##Oscilloscope", ImVec2(itemWidth, customH), ImPlotFlags_CanvasOnly)) {
				ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoTickLabels);
				ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_NoTickLabels);
				ImPlot::SetupAxisLimits(ImAxis_X1, 0.0, 500.0);
				ImPlot::SetupAxisLimits(ImAxis_Y1, -1.1, 1.1);
				auto size = std::min(data.size() - crossing, V);
				ImPlot::PlotLine("Signal", data.data() + crossing, static_cast<int>(size));
				ImPlot::EndPlot();
			}

		} else {
			ImGui::TextUnformatted("No input signal");
		}
	}

	inline float getCustomRenderingWidth() override {
		return customW;
	}

	inline float getCustomRenderingHeight() override {
		return customH;
	}

	static constexpr const char* circuitName = "Oscilloscope";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::probe;
	static constexpr float width = 300.0f;
	static constexpr float height = 150.0f;
	static constexpr size_t N = 1024;
	static constexpr size_t V = N / 2;

private:
	// properties
	OtCircuitPin input;

	float customW;
	float customH;

	std::mutex mutex;
	OtCircularBuffer<float, N> data;
};

static OtCircuitFactoryRegister<OtScope> registration;
