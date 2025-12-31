//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Based on https://github.com/martinfinke/PolyBLEP


#pragma once


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
		sawtooth
	};

	static constexpr const char* waveForms[] = {
		"Sine",
		"Square",
		"Triangle",
		"Saw Tooth"
	};

	static constexpr size_t waveFormCount = sizeof(waveForms) / sizeof(*waveForms);

	// set oscillator properties
	void setWaveForm(WaveForm wf) { waveForm = wf; }
	void setPitch(float p) { pitch = static_cast<double>(p); }
	void setPulseWidth(float pw);

	// get the next sample
	float get();

	// synchronize phase
	void synchronize(float phase);

private:
	// properties
	WaveForm waveForm = WaveForm::sine;
	double pitch = 440.0;
	double pulseWidth = 0.25;
	double t = 0.0;
};
