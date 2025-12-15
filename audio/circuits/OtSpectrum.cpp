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
#include <complex>
#include <mutex>

#include "imgui.h"
#include "implot.h"

#include "OtNumbers.h"

#include "OtAudioUtilities.h"
#include "OtCircuitFactory.h"
#include "OtFft.h"
#include "OtSignalQueue.h"


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
	inline void execute(size_t sr, size_t samples) override {
		if (input->isSourceConnected()) {
			sampleRate = static_cast<double>(sr);
			customW = width;
			customH = height;

			// add input signal to data buffer
			std::lock_guard<std::mutex> guard(mutex);
			auto signal = input->getSignalBuffer();
			data.insert(signal->data(), samples);

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
				// perform  Fast Fourier Transform (FFT)
				std::complex<float> fourier[fourierWindow] = {};

				{
					std::lock_guard<std::mutex> guard(mutex);

					for (size_t i = 0; i < signalWindow; i++) {
						fourier[i] = std::complex<float>(data[i] * hammingLut[i]);
					}
				}

				OtFft(fourier, fourierWindow);

				// determine magnitudes
				for (size_t i = 0; i < fourierWindow / 2; i++) {
					auto magnitude = std::sqrt(fourier[i].real() * fourier[i].real() + fourier[i].imag() * fourier[i].imag()) / fourierWindow;
					magnitudes[i] = std::lerp(magnitudes[i], magnitude, 0.05f);
				}

				// convert to decibels
				float spectrum[fourierWindow / 2] = {};

				for (size_t i = 0; i < fourierWindow / 2; i++) {
					spectrum[i] = OtLinearToDb(magnitudes[i]);
				}

				float freqBinSize = sampleRate / fourierWindow;
				ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
				ImPlot::SetupAxisLimits(ImAxis_X1, freqBinSize, sampleRate / 2.0, ImGuiCond_Always);
				ImPlot::SetupAxisTicks(ImAxis_X1, ticks, tickCount, tickLabels);
				ImPlot::SetupAxisLimits(ImAxis_Y1, -80.0, -10.0, ImGuiCond_Always);
				ImPlot::PlotShaded("", spectrum, static_cast<int>(fourierWindow / 2), -80.0, freqBinSize, freqBinSize);
				ImPlot::PlotLine("", spectrum, static_cast<int>(fourierWindow / 2), freqBinSize, freqBinSize);
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
	static constexpr size_t signalWindow = 512;	// signal buffer size
	static constexpr size_t fourierWindow = 4096;	// FFT buffer size

private:
	// properties
	OtCircuitPin input;

	float customW;
	float customH;
	float sampleRate;

	std::mutex mutex;
	OtSignalQueue<float, signalWindow> data;
	float magnitudes[fourierWindow / 2] = {};

	// hamming window support for FFT
	template<typename T, int N>
	inline constexpr std::array<T, N> generateLookupTable(T (*function)(int)) {
		std::array<T, N> lut;

		for (int i = 0; i < N; i++) {
			lut[i] = function(i);
		}

		return lut;
	}

	static float hammingWindow(int i) {
		return 0.53836f - 0.46164f * std::cos(std::numbers::pi2 * (1.0f / fourierWindow) * static_cast<float>(i));
	}

	std::array<float, signalWindow> hammingLut = generateLookupTable<float, signalWindow>(hammingWindow);
};

static OtCircuitFactoryRegister<OtSpectrum> registration;
