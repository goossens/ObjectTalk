//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtCircuitFactory.h"
#include "OtCompressor.h"
#include "OtFaust.h"


//
//	OtCompressorCircuit
//

class OtCompressorCircuit : public OtFaustCircuitMono<OtCompressor> {
public:
	static constexpr const char* circuitName = "Compressor";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::effect;
};

static OtCircuitFactoryRegister<OtCompressorCircuit> registration;
