//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtCircuitFactory.h"
#include "OtEq.h"
#include "OtFaust.h"


//
//	OtEqCircuit
//

class OtEqCircuit : public OtFaustEffectMono<OtEq> {
public:
	static constexpr const char* circuitName = "Equalizer";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::effect;
};

static OtCircuitFactoryRegister<OtEqCircuit> registration;
