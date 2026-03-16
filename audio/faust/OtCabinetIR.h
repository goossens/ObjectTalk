//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Based on https://github.com/brummer10/guitarix


#pragma once


//
//	Include files
//

#include <cmath>
#include <cstdint>


//
//	OtCabinetIR
//

struct OtCabinetIR {
	size_t size;
	float* data;
};


//
//	Cabinet definitions
//

enum class OtCabinetType {
	cab1x8,
	cab1x12,
	cab1x15,
	cab2x10,
	cab2x12,
	cab4x10,
	cab4x12,
	cabAC30,
	cabBassman,
	cabHighGain,
	cabMarshall,
	cabMesaBoogie,
	cabPrinceton,
	cabTwin
};


//
//	Tube table access functions
//

OtCabinetIR* OtCabinetIRGet(OtCabinetType type);
