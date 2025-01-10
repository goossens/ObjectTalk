//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtScriptAsset.h"


//
//	OtScriptAsset::compile
//

void OtScriptAsset::compile() {
	module = OtModule::create();
	module->load(path);
}
