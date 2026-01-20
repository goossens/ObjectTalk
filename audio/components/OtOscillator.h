//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Based on https://github.com/martinfinke/PolyBLEP


#pragma once


//
//	Include files
//

#include <cmath>
#include <string>

#include "nlohmann/json_fwd.hpp"

#include "OtAsset.h"

#include "OtAudioUtilities.h"
#include "OtSampleFile.h"
#include "OtSampleFileAsset.h"
#include "OtWaveTable.h"
#include "OtWaveTableAsset.h"


//
//	OtOscillator
//

class OtOscillator {
public:
	// waveform types
	enum class WaveForm {
		sine,
		square,
		triangle,
		sawtooth,
		sample,
		wavetable
	};

	static constexpr const char* waveForms[] = {
		"Sine",
		"Square",
		"Triangle",
		"Saw Tooth",
		"Sample",
		"Wave Table"
	};

	static constexpr size_t waveFormCount = sizeof(waveForms) / sizeof(*waveForms);

	// set of parameters that drive the oscillator
	class Parameters {
	public:
		// see if oscillator is ready to be used
		bool isReady();

		// UI to change oscillator properties
		bool renderUI();
		float getLabelWidth();
		float getRenderWidth();
		float getRenderHeight();

		// (de)serialize oscillator parameters
		void serialize(nlohmann::json* data, std::string* basedir);
		void deserialize(nlohmann::json* data, std::string* basedir);

		// properties
		WaveForm waveForm = WaveForm::sine;
		float frequency = 440.0f;
		float pulseWidth = 0.5f;
		float shape = 0.0f;
		float sampleFileFrequency = 0.0f;
		float tuningOctaves = 0.0f;
		float tuningSemitones = 0.0f;
		float tuningCents = 0.0f;
		float volume = 7.0f;

		OtAsset<OtSampleFileAsset> sampleFileAsset;
		OtSampleFile sampleFile;
		OtAsset<OtWaveTableAsset> waveTableAsset;
		OtWaveTable waveTable;

		bool showFrequencyKnob = false;
		float frequencyKnobLow = 0.0f;
		float frequencyKnobHigh = 0.0f;
		bool showPulseWidthKnob = false;
		bool showShapeKnob = false;
		bool showVolumeKnob = false;
		bool showTuningButton = false;
	};

	// state of the oscillator allowing multiple instances with identical parameters
	class State {
	public:
		// (re)set oscillator phase
		inline void synchronize(float phase=0.0f) {phase = OtAudioUtilities::fraction<float>(phase); }

		// properties
		float frequency = 440.0f;
		float pulseWidth = 0.5f;
		float shape = 0.0f;
		float phase = 0.0f;

		// get the next sample
		inline float get(Parameters& parameters) {
			float value;

			switch (parameters.waveForm) {
				case WaveForm::sine:
					value = sineFunction();
					updatePhase();
					break;

				case WaveForm::square:
					value = squareFunction(parameters.pulseWidth);
					updatePhase();
					break;

				case WaveForm::triangle:
					value = triangleFunction(frequency);
					updatePhase();
					break;

				case WaveForm::sawtooth:
					value = sawtoothFunction(frequency);
					updatePhase();
					break;

				case WaveForm::wavetable:
					value = wavetableFunction(parameters.waveTable, shape);
					updatePhase();
					break;

				default:
					value = 0.0f;
					break;
			}

			return value * parameters.volume / 10.0f;
		}

		// fill a buffer with next samples
		inline void get(Parameters& parameters, float* buffer, size_t size) {
			switch (parameters.waveForm) {
				case WaveForm::sine:
					for (size_t i = 0; i < size; i++) {
						*buffer++ = sineFunction() * parameters.volume / 10.0f;
						updatePhase();
					}

					break;

				case WaveForm::square:
					for (size_t i = 0; i < size; i++) {
						*buffer++ = squareFunction(parameters.pulseWidth) * parameters.volume / 10.0f;
						updatePhase();
					}

					break;

				case WaveForm::triangle:
					for (size_t i = 0; i < size; i++) {
						*buffer++ = triangleFunction(frequency) * parameters.volume / 10.0f;
						updatePhase();
					}

					break;

				case WaveForm::sawtooth:
					for (size_t i = 0; i < size; i++) {
						*buffer++ = sawtoothFunction(frequency) * parameters.volume / 10.0f;
						updatePhase();
					}

					break;

				case WaveForm::wavetable:
					for (size_t i = 0; i < size; i++) {
						*buffer++ = wavetableFunction(parameters.waveTable, shape) * parameters.volume / 10.0f;
						updatePhase();
					}

					break;

				default:
					for (size_t i = 0; i < size; i++) {
						*buffer++ = 0.0f;
					}

					break;
			}
		}

	private:
		// oscillator functions
		inline float sineFunction() {
			return std::sin(OtAudioSettings::pi2 * phase);
		}

		inline float squareFunction(float pulseWidth) {
			return phase < pulseWidth ? 1.0f : -1.0f;
		}

		inline float triangleFunction(float frequency) {
			auto t1 = OtAudioUtilities::fraction<float>(phase + 0.25f);
			auto t2 = OtAudioUtilities::fraction<float>(phase + 0.75f);
			auto y = phase * 4.0f;

			if (y >= 3.0f) {
				y -= 4.0f;

			} else if (y > 1.0f) {
				y = 2.0f - y;
			}

			auto dt = frequency * OtAudioSettings::dt;
			return y + 4 * dt * (OtAudioUtilities::blamp<float>(t1, dt) - OtAudioUtilities::blamp<float>(t2, dt));
		}

		inline float sawtoothFunction(float frequency) {
				auto t1 = OtAudioUtilities::fraction<float>(phase + 0.5f);
				auto y = 2.0f * t1 - 1.0f;
				return y - OtAudioUtilities::blep<float>(t1, frequency * OtAudioSettings::dt);
		}

		inline float wavetableFunction(OtWaveTable& waveTable, float shape) {
			if (waveTable.isValid()) {
				return waveTable.get(phase, shape);

			} else {
				return 0.0f;
			}
		}

		// update the oscillator phase
		inline void updatePhase() {
			phase += frequency * OtAudioSettings::dt;

			if (phase > 1.0f) {
				phase -= 1.0f;
			}
		}
	};
};
