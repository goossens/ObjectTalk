//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "simulation.h"


//
//	OtSimulationClass::getMeta
//

OtType OtSimulationClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtSimulationClass>("Simulation", OtGuiClass::getMeta());
		type->set("start", OtFunctionClass::create(&OtSimulationClass::start));
		type->set("stop", OtFunctionClass::create(&OtSimulationClass::stop));
		type->set("isRunning", OtFunctionClass::create(&OtSimulationClass::isRunning));
	}

	return type;
}
