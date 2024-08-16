//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtSingleton.h"


//
//	OtConfig
//

class OtConfig : public OtSingleton<OtConfig> {
public:
	// access debug mode
	void setDebugMode(bool flag) { debugMode = flag; }
	bool inDebugMode() { return debugMode; }

	// access subprocess mode
	void setSubprocessMode(bool flag) { subprocessMode = flag; }
	bool inSubprocessMode() { return subprocessMode; }

private:
	// configuration
	bool debugMode = false;
	bool subprocessMode = false;
};
