//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtCircuitFactory.h"


//
//	OtMeterBridge
//

class OtMeterBridge : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		input1 = addInputPin("Input 1", OtCircuitPinClass::Type::mono)->hasMeter();
		input2 = addInputPin("Input 2", OtCircuitPinClass::Type::mono)->hasMeter();
		input3 = addInputPin("Input 3", OtCircuitPinClass::Type::mono)->hasMeter();
		input4 = addInputPin("Input 4", OtCircuitPinClass::Type::mono)->hasMeter();
		input5 = addInputPin("Input 5", OtCircuitPinClass::Type::mono)->hasMeter();
		input6 = addInputPin("Input 6", OtCircuitPinClass::Type::mono)->hasMeter();
		input7 = addInputPin("Input 7", OtCircuitPinClass::Type::mono)->hasMeter();
		input8 = addInputPin("Input 8", OtCircuitPinClass::Type::mono)->hasMeter();
	}

	static constexpr const char* circuitName = "Meter Bridge";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::probe;

private:
	// properties
	OtCircuitPin input1;
	OtCircuitPin input2;
	OtCircuitPin input3;
	OtCircuitPin input4;
	OtCircuitPin input5;
	OtCircuitPin input6;
	OtCircuitPin input7;
	OtCircuitPin input8;
};

static OtCircuitFactoryRegister<OtMeterBridge> registration;
