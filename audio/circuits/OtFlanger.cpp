//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFlanger.h"
#include "OtCircuitFactory.h"
#include "OtFaust.h"


//
//	OtFlangerCircuit
//

class OtFlangerCircuit : public OtFaustCircuitMono<OtFlanger> {
public:
	static constexpr const char* circuitName = "Flanger";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::effect;
};

static OtCircuitFactoryRegister<OtFlangerCircuit> registration;
