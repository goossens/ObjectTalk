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
#include "OtTubePre12AX7.h"


//
//	OtTubePre12AX7Circuit
//

class OtTubePre12AX7Circuit : public OtFaustEffectMono<OtTubePre12AX7> {
public:
	static constexpr const char* circuitName = "Tube Preamp 12AX7";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::processor;
};

static OtCircuitFactoryRegister<OtTubePre12AX7Circuit> registration;
