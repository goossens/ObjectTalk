//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtModule.h"

#include "OtTextAsset.h"


//
//	OtScriptAsset
//

class OtScriptAsset : public OtTextAsset {
public:
	// compile the script (text) into a module
	void compile();

	// access the scrip module
	OtModule& getModule() { return module; }

private:
	// the compiled module
	OtModule module;
};
