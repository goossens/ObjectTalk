//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtCircuitFactory.h"


//
//	OtSpeakerOut
//

class OtSpeakerOut : public OtCircuitClass {
public:
	// configure node
	inline void configure() override {
		input = addInputPin("Input", OtCircuitPinClass::Type::stereo);
	}

	static constexpr const char* circuitName = "Speaker Out";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::output;

private:
	// properties
	OtCircuitPin input;
};

static OtCircuitFactoryRegister<OtSpeakerOut> registration;
