//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtCircuitFactory.h"
#include "OtFaust.h"
#include "OtFlanger.h"


//
//	OtFlangerCircuit
//

class OtFlangerCircuit : public OtFaustCircuitMono<OtFlanger> {
public:
	static constexpr const char* circuitName = "Flanger";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::effect;
};

static OtCircuitFactoryRegister<OtFlangerCircuit> registration;
