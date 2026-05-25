//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtAutoWah.h"
#include "OtCircuitFactory.h"
#include "OtFaust.h"


//
//	OtAutoWahCircuit
//

class OtAutoWahCircuit : public OtFaustEffectMono<OtAutoWah> {
public:
	static constexpr const char* circuitName = "Auto Wah";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::effect;
};

static OtCircuitFactoryRegister<OtAutoWahCircuit> registration;
