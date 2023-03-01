//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"

#include "OtSimulation.h"


//
//	OtSimulationClass::getMeta
//

OtType OtSimulationClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtSimulationClass>("Simulation", OtGuiClass::getMeta());
		type->set("start", OtFunction::create(&OtSimulationClass::start));
		type->set("stop", OtFunction::create(&OtSimulationClass::stop));
		type->set("isRunning", OtFunction::create(&OtSimulationClass::isRunning));
	}

	return type;
}
