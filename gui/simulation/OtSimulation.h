//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtGui.h"


//
//	OtSimulationClass
//

class OtSimulationClass;
using OtSimulation = OtObjectPointer<OtSimulationClass>;

class OtSimulationClass : public OtGuiClass {
public:
	// start stop simulation
	OtObject start() { running = true; return OtObject(this); }
	OtObject stop() { running = false; return OtObject(this); }
	bool isRunning() { return running; }

	// perform simulation step
	virtual void step(int32_t deltaMilliseconds) {}

	// get type definition
	static OtType getMeta();

private:
	bool running = false;
};
