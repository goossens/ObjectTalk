//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>
#include <mutex>

#include "imgui.h"
#include "implot.h"

#include "OtNumbers.h"

#include "OtAudioUtilities.h"
#include "OtCircuitFactory.h"
#include "OtFft.h"
#include "OtCircularBuffer.h"


//
//	OtSpectrum
//

class OtSpectrum : public OtCircuitClass {
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
			data.insert(signal->data(), signal->getSampleCount());

		} else {
			customW = width;
			customH = ImGui::GetFrameHeightWithSpacing();

		}

		needsSizing = true;
	}

	// render custom fields
	inline void customRendering(float itemWidth) override {
		static constexpr double ticks[] = {
			16.0, 31.0, 62.0, 125.0, 250.0, 500.0,
			1000.0, 2000.0, 4000.0, 8000.0, 16000.0
		};

		static constexpr char const * tickLabels[] = {
			"16",  "31",  "62",  "125", "250", "500",
			"1K", "2K",  "4K",  "8K",  "16K"
		};

		static constexpr int tickCount = sizeof(ticks) / sizeof(*ticks);

		if (input->isSourceConnected()) {
			ImPlot::PushStyleColor(ImPlotCol_AxisText, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
			ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);

			if (ImPlot::BeginPlot("##Spectrum", ImVec2(itemWidth, customH), ImPlotFlags_CanvasOnly)) {
				{
					// quickly copy signal to separate buffer
					// to avoid race conditions and to not hold up audio thread
					std::lock_guard<std::mutex> guard(mutex);
					std::copy(data.begin(), data.end(), fftInputBuffer);
				}

				// perform  Fast Fourier Transform (FFT)
				auto result = fft.forwardFrequencyOnly(fftInputBuffer);

				// smooth out magnitude changes
				for (size_t i = 0; i < N / 2; i++) {
					magnitudes[i] = std::lerp(magnitudes[i], result[i], 0.1f);
				}

				// display frequency spectrum
				float freqBinSize = OtAudioSettings::sampleRate / N;
				ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
				ImPlot::SetupAxisLimits(ImAxis_X1, freqBinSize, OtAudioSettings::sampleRate / 2.0, ImGuiCond_Always);
				ImPlot::SetupAxisTicks(ImAxis_X1, ticks, tickCount, tickLabels);
				ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_NoTickLabels);
				ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, 0.8, ImGuiCond_Always);
				ImPlot::PlotShaded("", magnitudes + 1, static_cast<int>(N / 2 - 1), 0.0, freqBinSize, freqBinSize);
				ImPlot::PlotLine("", magnitudes + 1, static_cast<int>(N / 2 - 1), freqBinSize, freqBinSize);
				ImPlot::EndPlot();
			}

			ImPlot::PopStyleVar();
			ImPlot::PopStyleColor();

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

	static constexpr const char* circuitName = "Spectrum Analyzer";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::probe;
	static constexpr float width = 600.0f;
	static constexpr float height = 250.0f;
	static constexpr size_t N = 4096;

private:
	// properties
	OtCircuitPin input;

	float customW;
	float customH;

	OtFft fft{N};

	std::mutex mutex;
	OtCircularBuffer<float, N> data;
	float fftInputBuffer[N];
	float magnitudes[N / 2] = {};
};

static OtCircuitFactoryRegister<OtSpectrum> registration;
