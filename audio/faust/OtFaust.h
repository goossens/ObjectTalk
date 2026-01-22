//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstring>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "nlohmann/json_fwd.hpp"

#include "OtAssert.h"

#include "OtAudioSettings.h"
#include "OtCircuitFactory.h"
#include "OtAudioBuffer.h"


//
//	OtFaust
//

class OtFaust {
protected:
	// Faust metadata collector
	class Meta {
	public:
		// add metadata
		void declare(const char* name, const char* value) {
			metadata[name] = value;
		}

	private:
		// properties
		std::unordered_map<std::string, std::string> metadata;
	};

	// unused for now
	struct Soundfile{};

	// interface builder
	class UiElement {
	public:
		// element types
		enum class Type {
			box,
			horizontal,
			vertical,
			close,
			button,
			control
		};

		// constructors
		UiElement(Type t, const char* l="") : type(t), label(l) {}
		UiElement(const char* l, float* v) : type(Type::button), label(l), variable(v), def(*v), max(1.0f) {}

		UiElement(const char* l, float* v, float d, float mn, float mx, float s) :
			type(Type::control),
			label(l),
			variable(v),
			def(d),
			min(mn),
			max(mx),
			step(s)	{}

		// post process metadata declarations and other values
		inline void processMetaData(std::unordered_map<std::string, std::string>& data) {
			if (data.find("format") != data.end()) { format = data["format"]; }
			if (data.find("off") != data.end()) { offLabel = data["off"]; }
			if (data.find("on") != data.end()) { onLabel = data["on"]; }

			if (format.size() == 0) {
				if (step <= 0.01f) {
					format = "%.2f";

				} else if (step <= 0.1f) {
					format = "%.1f";

				} else {
					format = "%.0f";
				}
			}
		}

		// access properties
		inline Type getType() { return type; }
		inline const char* getLabel() { return label.c_str(); }
		inline const char* getFormat() { return format.c_str(); }
		inline float* getVariable() { return variable; }
		inline float getDefault() { return def; }
		inline float getMin() { return min; }
		inline float getMax() { return max; }
		inline float getStep() { return step; }

		inline const char* getOnLabel() { return onLabel.c_str(); }
		inline const char* getOffLabel() { return offLabel.c_str(); }

	private:
		// properties
		Type type;
		std::string label;
		std::string format;
		float* variable = nullptr;
		float def = 0.0f;
		float min = 0.0f;
		float max = 0.0f;
		float step = 0.0f;

		std::string onLabel;
		std::string offLabel;
	};

	class UI {
	public:
		// UI methods
		void openTabBox(const char* label) { elements.emplace_back(UiElement::Type::box, label); }
		void openHorizontalBox(const char* label) { elements.emplace_back(UiElement::Type::horizontal, label); }
		void openVerticalBox(const char* label) { elements.emplace_back(UiElement::Type::vertical, label); }
		void closeBox() { elements.emplace_back(UiElement::Type::close); }

		void addButton([[maybe_unused]] const char* label, [[maybe_unused]] float* value) {}
		void addCheckButton(const char* label, float* value) { elements.emplace_back(label, value); }
		void addVerticalSlider(const char* label, float* value, float init, float min, float max, float step) { elements.emplace_back(label, value, init, min, max, step); }
		void addHorizontalSlider(const char* label, float* value, float init, float min, float max, float step) { elements.emplace_back(label, value, init, min, max, step); }
		void addNumEntry(const char* label, float* value, float init, float min, float max, float step) { elements.emplace_back(label, value, init, min, max, step); }

		void addHorizontalBargraph([[maybe_unused]] const char* name, [[maybe_unused]] float* value, [[maybe_unused]] float min, [[maybe_unused]] float max) {}
		void addVerticalBargraph([[maybe_unused]] const char* name, [[maybe_unused]] float* value, [[maybe_unused]] float min, [[maybe_unused]] float max) {}

		void addSoundfile([[maybe_unused]] const char* name, [[maybe_unused]] const char* filename, [[maybe_unused]] Soundfile** sf) {}

		void declare(float* variable, const char* key, const char* value) {
			metadata[variable][key] = value;
		}

		// post process all elements
		void postProcessUiElements() {
			// process metadata (if required) and determine maximum number of rows and columns
			for (auto& element : elements) {
				element.processMetaData(metadata[element.getVariable()]);

				switch (element.getType()) {
					case UiElement::Type::vertical:
						rows++;
						break;

					case UiElement::Type::button:
					case UiElement::Type::control:
						cols++;
						break;

					default:
						break;
				}
			}
		}

		// UI properties
		std::unordered_map<float*, std::unordered_map<std::string, std::string>> metadata;
		std::vector<UiElement> elements;
		size_t rows = 0;
		size_t cols = 0;
	};

	Meta meta;
	UI ui;

public:
	// destructor
	virtual ~OtFaust() {}

	// initialize processor
	void initialize() {
		init(OtAudioSettings::sampleRate);
		metadata(&meta);
		buildUserInterface(&ui);
		ui.postProcessUiElements();
	}

	// collect metadata
	virtual void metadata(Meta* meta) = 0;

	// return number of audio inputs/outputs
	virtual int getNumInputs() = 0;
	virtual int getNumOutputs() = 0;

	// construct a user interface
	virtual void buildUserInterface(UI* ui) = 0;

	// return the sample rate
	virtual int getSampleRate() = 0;

	// initialize the DSP unit
	virtual void init(int sampleRate) = 0;

	// run the Faust DSP unit
	virtual void compute(int count, float** inputs, float** outputs) = 0;

	// user interface
	bool render();
	float getWidth();
	float getHeight();

	// (de)serialize variables
	void serialize(nlohmann::json* data);
	void deserialize(nlohmann::json* data);
};


//
//	OtFaustComponent
//

template<typename T>
class OtFaustComponent {
public:
	// set of parameters that drive the component
	class Parameters {
	public:
		Parameters() { dsp.initialize(); }

		// UI to change component properties
		inline bool renderUI(){ return dsp.render(); }
		inline float getRenderWidth() { return dsp.getWidth(); }
		inline float getRenderHeight() { return dsp.getHeight(); }

		// (de)serialize filter parameters
		inline void serialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) { dsp.serialize(data); }
		inline void deserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) { dsp.deserialize(data); }

		// process samples
		inline void process(float* buffer, size_t size) { dsp.compute(static_cast<int>(size), &buffer, &buffer); };

		// target Faust processor
		T dsp;
	};

	// state of the component allowing multiple instances with identical parameters
	class State {
	public:
		// target Faust processor
		T dsp;
	};
};


//
//	OtFaustCircuitMono
//

template<typename T>
class OtFaustCircuitMono : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		dsp.initialize();

		OtAssert(dsp.getNumInputs() == 1);
		OtAssert(dsp.getNumOutputs() == 1);

		audioInput = addInputPin("Input", OtCircuitPinClass::Type::mono);
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono);
	}

	// render custom fields
	inline bool customRendering([[maybe_unused]] float itemWidth) override {
		return dsp.render();
	}

	inline float getCustomRenderingWidth() override {
		return dsp.getWidth();
	}

	inline float getCustomRenderingHeight() override {
		return dsp.getHeight();
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		dsp.serialize(data);
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		dsp.deserialize(data);
	}

	// process samples
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			if (audioInput->isSourceConnected()) {
				auto in = audioInput->getSamples();
				auto out = audioOutput->getAudioOutputBuffer().data();
				dsp.compute(OtAudioSettings::bufferSize, &in, &out);

			} else {
				audioOutput->setSamples(0.0f);
			}
		}
	};

private:
	// circuit work variables
	OtCircuitPin audioInput;
	OtCircuitPin audioOutput;

	// target Faust processor
	T dsp;
};


//
//	OtFaustCircuitStereo
//

template<typename T>
class OtFaustCircuitStereo : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		dsp.initialize();

		OtAssert(dsp.getNumInputs() == 2);
		OtAssert(dsp.getNumOutputs() == 2);

		audioInput = addInputPin("Input", OtCircuitPinClass::Type::stereo);
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::stereo);
	}

	// render custom fields
	inline bool customRendering([[maybe_unused]] float itemWidth) override {
		return dsp.render();
	}

	inline float getCustomRenderingWidth() override {
		return dsp.getWidth();
	}

	inline float getCustomRenderingHeight() override {
		return dsp.getHeight();
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		dsp.serialize(data);
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		dsp.deserialize(data);
	}

	// process samples
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			if (audioInput->isSourceConnected()) {
				float* inputs[] = { leftIn.data(), rightIn.data() };
				float* outputs[] = { leftOut.data(), rightOut.data() };

				deinterleave(audioInput->getAudioInputBuffer(), leftIn, rightIn);
				dsp.compute(OtAudioSettings::bufferSize, inputs, outputs);
				interleave(audioOutput->getAudioOutputBuffer(), leftOut, rightOut);

			} else {
				audioOutput->setSamples(0.0f);
			}
		}
	};

private:
	// circuit work variables
	OtCircuitPin audioInput;
	OtCircuitPin audioOutput;

	OtAudioBuffer leftIn{1};
	OtAudioBuffer rightIn{1};
	OtAudioBuffer leftOut{1};
	OtAudioBuffer rightOut{1};

	// target Faust processor
	T dsp;

	// interleave two mono buffers into one stereo buffer
	void interleave(OtAudioBuffer& stereo, OtAudioBuffer& left, OtAudioBuffer& right) {
		auto sp = stereo.data();
		auto lp = left.data();
		auto rp = right.data();

		for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
			*sp++ = *lp++;
			*sp++ = *rp++;
		}
	}

	// de-interleave a stereo buffer into two mono buffers
	void deinterleave(OtAudioBuffer& stereo, OtAudioBuffer& left, OtAudioBuffer& right) {
		auto sp = stereo.data();
		auto lp = left.data();
		auto rp = right.data();

		for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
			*lp++ = *sp++;
			*rp++ = *sp++;
		}
	}
};
