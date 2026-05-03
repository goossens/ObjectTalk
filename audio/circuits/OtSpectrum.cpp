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

#include "imgui.h"
#include "implot.h"

#include "OtAudioSettings.h"
#include "OtCircuitFactory.h"
#include "OtFft.h"
#include "OtHammingWindow.h"
#include "OtCircularBuffer.h"


//
//	OtSpectrum
//

class OtSpectrum : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		input = addInputPin("Input", OtCircuitPinClass::Type::mono)->hasAttenuation();
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
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

			if (ImPlot::BeginPlot("##Spectrum", ImVec2(itemWidth, height), ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs)) {
				{
					// quickly copy signal to separate buffer
					// to avoid race conditions and to not hold up audio thread
					std::lock_guard<std::mutex> guard(mutex);
					std::copy(data.begin(), data.end(), fftInputBuffer);
				}

				// apply hamming window
				hammingWindow.apply(fftInputBuffer);

				// perform Fast Fourier Transform (FFT)
				auto result = fft.forwardFrequencyOnly(fftInputBuffer);

				// smooth out magnitude changes
				for (size_t i = 0; i < N / 2; i++) {
					magnitudes[i] = std::lerp(magnitudes[i], result[i], 0.1f);
				}

				// convert to dBFS
				float dbfs[N / 2];

				for (size_t i = 0; i < N / 2; i++) {
					dbfs[i] = OtAudioUtilities::linearToDbfs(magnitudes[i]);
				}

				// display frequency spectrum
				ImGui::PushFont(nullptr, ImGui::GetTextLineHeight() - 4.0f);
				float freqBinSize = OtAudioSettings::sampleRate / N;
				ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
				ImPlot::SetupAxisLimits(ImAxis_X1, freqBinSize, OtAudioSettings::sampleRate / 2.0, ImGuiCond_Always);
				ImPlot::SetupAxisTicks(ImAxis_X1, ticks, tickCount, tickLabels);
				ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_NoTickLabels);
				ImPlot::SetupAxisLimits(ImAxis_Y1, -60.0, 0.0, ImGuiCond_Always);
				ImPlotSpec spec;
				spec.FillAlpha = 0.5f;
				ImPlot::PlotShaded("", dbfs, static_cast<int>(N / 2), -60.0, freqBinSize, freqBinSize, spec);
				ImPlot::PlotLine("", dbfs, static_cast<int>(N / 2), freqBinSize, freqBinSize);
				ImPlot::EndPlot();
				ImGui::PopFont();
			}

			ImPlot::PopStyleColor();

		} else {
			OtUi::text("No input signal");
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
			customH = height + ImGui::GetStyle().ItemSpacing.y;

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

	static constexpr const char* circuitName = "Spectrum Analyzer";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::probe;
	static constexpr float width = 300.0f;
	static constexpr float height = 150.0f;
	static constexpr size_t N = 4096;

private:
	// properties
	OtCircuitPin input;

	float customW;
	float customH;

	OtHammingWindow hammingWindow{N};
	OtFft fft{N};

	std::mutex mutex;
	OtCircularBuffer<float, N> data;
	float fftInputBuffer[N];
	float magnitudes[N / 2] = {};
};

static OtCircuitFactoryRegister<OtSpectrum> registration;
