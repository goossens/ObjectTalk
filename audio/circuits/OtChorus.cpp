//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtChorus.h"
#include "OtCircuitFactory.h"
#include "OtFaust.h"


//
//	OtChorusCircuit
//

class OtChorusCircuit : public OtFaustCircuitMono<OtChorus> {
public:
	static constexpr const char* circuitName = "Chorus";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::effect;
};

static OtCircuitFactoryRegister<OtChorusCircuit> registration;
