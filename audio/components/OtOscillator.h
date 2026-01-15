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
		OtAsset<OtSampleFileAsset> sampleFileAsset;
		OtSampleFile sampleFile;
		OtAsset<OtWaveTableAsset> waveTableAsset;
		OtWaveTable waveTable;

		bool showFrequencyKnob = false;
		float frequencyKnobLow = 0.0f;
		float frequencyKnobHigh = 0.0f;
		bool showPulseWidthKnob = false;
		bool showShapeKnob = false;
	};

	// state of the oscillator allowing multiple instances with identical parameters
	class State {
	public:
		// (re)set oscillator phase
		inline void synchronize(float phase=0.0f) {phase = OtAudioUtilities::fraction<float>(phase); }

		// properties
		float frequency = 0.0f;
		float pulseWidth = 0.5f;
		float shape = 0.0f;
		float phase = 0.0f;
	};

	// get the next sample
	inline static float get(Parameters& parameters, State& state) {
		float value;

		switch (parameters.waveForm) {
			case WaveForm::sine:
				value = sineFunction(state.phase);
				updatePhase(state);
				break;

			case WaveForm::square: {
				value = squareFunction(state.phase, parameters.pulseWidth);
				updatePhase(state);
				break;
			}

			case WaveForm::triangle: {
				value = triangleFunction(state.phase, state.frequency);
				updatePhase(state);
				break;
			}

			case WaveForm::sawtooth: {
				value = sawtoothFunction(state.phase, state.frequency);
				updatePhase(state);
				break;
			}

			case WaveForm::wavetable: {
				value = wavetableFunction(parameters.waveTable, state.phase, state.shape);
				updatePhase(state);
				break;
			}

			default:
				value = 0.0f;
				break;
		}

		return value;
	}

	// fill a buffer with samples
	inline static void get(float* buffer, size_t size, Parameters& parameters, State& state) {
		switch (parameters.waveForm) {
			case WaveForm::sine:
				for (size_t i = 0; i < size; i++) {
					*buffer++ = sineFunction(state.phase);
					updatePhase(state);
				}

				break;

			case WaveForm::square: {
				for (size_t i = 0; i < size; i++) {
					*buffer++ = squareFunction(state.phase, parameters.pulseWidth);
					updatePhase(state);
				}

				break;
			}

			case WaveForm::triangle: {
				for (size_t i = 0; i < size; i++) {
					*buffer++ = triangleFunction(state.phase, state.frequency);
					updatePhase(state);
				}

				break;
			}

			case WaveForm::sawtooth: {
				for (size_t i = 0; i < size; i++) {
					*buffer++ = sawtoothFunction(state.phase, state.frequency);
					updatePhase(state);
				}

				break;
			}

			case WaveForm::wavetable: {
				for (size_t i = 0; i < size; i++) {
					*buffer++ = wavetableFunction(parameters.waveTable, state.phase, state.shape);
					updatePhase(state);
				}

				break;
			}

			default:
				for (size_t i = 0; i < size; i++) {
					*buffer++ = 0.0f;
				}

				break;
		}
	}

private:
	// oscillator functions
	inline static float sineFunction(float phase) {
		return std::sin(OtAudioSettings::pi2 * phase);
	}

	inline static float squareFunction(float phase, float pulseWidth) {
		return phase < pulseWidth ? 1.0f : -1.0f;
	}

	inline static float triangleFunction(float phase, float frequency) {
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

	inline static float sawtoothFunction(float phase, float frequency) {
			auto t1 = OtAudioUtilities::fraction<float>(phase + 0.5f);
			auto y = 2.0f * t1 - 1.0f;
			return y - OtAudioUtilities::blep<float>(t1, frequency * OtAudioSettings::dt);
	}

	inline static float wavetableFunction(OtWaveTable& waveTable, float phase, float shape) {
		if (waveTable.isValid()) {
			return waveTable.get(phase, shape);

		} else {
			return 0.0f;
		}
	}

	// update the oscillator phase
	inline static void updatePhase(State& state) {
		state.phase += state.frequency * OtAudioSettings::dt;

		if (state.phase > 1.0f) {
			state.phase -= 1.0f;
		}
	}
};
