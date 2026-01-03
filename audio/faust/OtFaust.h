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
private:
	// work variables
	OtCircuitPin pitchInput;
	OtCircuitPin audioOutput;

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
	class UI {
	public:
		// UI methods
		void openTabBox(const char* boxLabel) { pushName(boxLabel); }
		void openHorizontalBox(const char* boxLabel) { pushName(boxLabel); }
		void openVerticalBox(const char* boxLabel) { pushName(boxLabel); }
		void closeBox() { popName(); }

		void addButton(const char* name, float* value) { addControl(name, value, *value, 0.0f, 1.0f); }
		void addCheckButton(const char* name, float* value) { addControl(name, value, *value, 0.0f, 1.0f); }
		void addVerticalSlider(const char* name, float* value, float init, float min, float max, float step) { addControl(name, value, init, min, max, step); }
		void addHorizontalSlider(const char* name, float* value, float init, float min, float max, float step) { addControl(name, value, init, min, max, step); }
		void addNumEntry(const char* name, float* value, float init, float min, float max, float step) { addControl(name, value, init, min, max, step); }

		void addHorizontalBargraph([[maybe_unused]] const char* name, [[maybe_unused]] float* value, [[maybe_unused]] float min, [[maybe_unused]] float max) {}
		void addVerticalBargraph([[maybe_unused]] const char* name, [[maybe_unused]] float* value, [[maybe_unused]] float min, [[maybe_unused]] float max) {}

		void addSoundfile([[maybe_unused]] const char* name, [[maybe_unused]] const char* filename, [[maybe_unused]] Soundfile** sf) {}

		void declare([[maybe_unused]] float* variable, const char* key, const char* value) {
			if (std::strcmp(key, "alias") == 0) {
				aliasing = true;

			} else if (std::strcmp(key, "format") == 0) {
				format = value;

			} else if (std::strcmp(key, "name") == 0) {
				label = value;
			}
		}

		// control definition
		class Control {
		public:
			Control(
				const std::string& n,
				const std::string& l,
				const std::string& f,
				float* v,
				float d,
				float mn,
				float mx,
				float s=1.0f,
				const char** sn=nullptr) :
					name(n),
					label(l),
					format(f),
					variable(v),
					def(d),
					min(mn),
					max(mx),
					step(s),
					stepNames(sn) {

				// set label to name if not specified
				if (label.size() == 0) {
					label = name;
				}

				// set default value
				*v = def;

				// determine formating base on step size
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

			const char* getName() { return name.c_str(); }
			const char* getLabel() { return label.c_str(); }
			const char* getFormat() { return format.c_str(); }
			float* getVariable() { return variable; }
			float getDefault() { return def; }
			float getMin() { return min; }
			float getMax() { return max; }
			float getStep() { return step; }
			const char* getStepName(int seqno) { return stepNames ? stepNames[seqno] : ""; }

			float get() { return *variable; }

			void set(float value) {
				*variable = value;

				for (auto i = subControls.begin(); i != subControls.end(); i++)
					(*i)->set(value);
			}

			void addControl(std::shared_ptr<Control> control) {
				subControls.push_back(control);
			}

			std::string name;
			std::string label;
			std::string format;
			float* variable;
			float def;
			float min;
			float max;
			float step;
			const char** stepNames;

			std::vector<std::shared_ptr<Control>> subControls;
		};

		// update label prefix
		void updatePrefix() {
			prefix = "";

			for (auto& entry : prefixStack) {
				prefix += entry + ".";
			}
		}

		// add a new prefix label
		void pushName(const char* boxLabel) {
			prefixStack.push_back(boxLabel);
			updatePrefix();
		}

		// remove the last prefix label
		void popName() {
			prefixStack.pop_back();
			updatePrefix();
		}

		void addControl(const char* name, float* variable, float def, float min, float max, float step=1.0f, const char** stepNames=nullptr) {
			std::string prefixedName = prefix + name;

			if (aliasing) {
				if (index.find(name) == index.end()) {
					auto group = std::make_shared<Control>(prefixedName, label, format, variable, def, min, max, step, stepNames);
					controls.push_back(group);
					index[name] = group;
				}

				index.find(name)->second->addControl(std::make_shared<Control>(prefixedName, label, format, variable, def, min, max, step, stepNames));
				aliasing = false;

			} else {
				auto control = std::make_shared<Control>(name, label, format, variable, def, min, max, step, stepNames);
				controls.push_back(control);
				index[prefixedName] = control;
			}

			label.clear();
			format.clear();
		}

		// UI properties
		std::vector<std::shared_ptr<Control>> controls;
		std::unordered_map<std::string, std::shared_ptr<Control>> index;

		std::vector<std::string> prefixStack;
		std::string prefix;
		bool aliasing = false;
		std::string format;
		std::string label;
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
//	OtFaustCircuitMono
//

template<typename T>
class OtFaustCircuitMono : public OtCircuitClass {
public:
	// configure node
	inline void configure() override {
		faustDsp.initialize();

		OtAssert(faustDsp.getNumInputs() == 1);
		OtAssert(faustDsp.getNumOutputs() == 1);

		audioInput = addInputPin("Input", OtCircuitPinClass::Type::mono);
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono);
	}

	// render custom fields
	inline bool customRendering([[maybe_unused]] float itemWidth) override {
		return faustDsp.render();
	}

	inline float getCustomRenderingWidth() override {
		return faustDsp.getWidth();
	}

	inline float getCustomRenderingHeight() override {
		return faustDsp.getHeight();
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		faustDsp.serialize(data);
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		faustDsp.deserialize(data);
	}

	// process samples
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			if (audioInput->isSourceConnected()) {
				auto in = audioInput->getSamples();
				auto out = audioOutput->getOutputBuffer().data();
				faustDsp.compute(OtAudioSettings::bufferSize, &in, &out);

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
	T faustDsp;
};


//
//	OtFaustCircuitStereo
//

template<typename T>
class OtFaustCircuitStereo : public OtCircuitClass {
public:
	// configure node
	inline void configure() override {
		faustDsp.initialize();

		OtAssert(faustDsp.getNumInputs() == 2);
		OtAssert(faustDsp.getNumOutputs() == 2);

		audioInput = addInputPin("Input", OtCircuitPinClass::Type::stereo);
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::stereo);
	}

	// render custom fields
	inline bool customRendering([[maybe_unused]] float itemWidth) override {
		return faustDsp.render();
	}

	inline float getCustomRenderingWidth() override {
		return faustDsp.getWidth();
	}

	inline float getCustomRenderingHeight() override {
		return faustDsp.getHeight();
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		faustDsp.serialize(data);
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		faustDsp.deserialize(data);
	}

	// process samples
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			if (audioInput->isSourceConnected()) {
				float* inputs[] = { leftIn.data(), rightIn.data() };
				float* outputs[] = { leftOut.data(), rightOut.data() };

				deinterleave(audioInput->getInputBuffer(), leftIn, rightIn);
				faustDsp.compute(OtAudioSettings::bufferSize, inputs, outputs);
				interleave(audioOutput->getOutputBuffer(), leftOut, rightOut);

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
	T faustDsp;

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
