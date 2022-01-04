//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "gui.h"


//
//	OtSimulationClass
//

class OtSimulationClass;
typedef std::shared_ptr<OtSimulationClass> OtSimulation;

class OtSimulationClass : public OtGuiClass {
public:
	// start stop simulation
	OtObject start() { running = true; return shared(); }
	OtObject stop() { running = false; return shared(); }
	bool isRunning() { return running; }

	// perform simulation step
	virtual void step(int32_t deltaMilliseconds) {}

	// get type definition
	static OtType getMeta();

private:
	bool running = false;
};
