//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

class OtConfig : OtSingleton<OtConfig> {
public:
	// access subprocess mode
	static inline void setSubprocessMode(bool flag) { instance().subprocessMode = flag; }
	static inline bool inSubprocessMode() { return instance().subprocessMode; }

private:
	// configuration
	bool subprocessMode = false;
};
