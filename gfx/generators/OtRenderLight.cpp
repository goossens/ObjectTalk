//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtRenderLight.h"


//
//	OtRenderLight::preparePass
//

OtComputeProgram& OtRenderLight::preparePass() {
	uniform.setValue(0, center, size);
	uniform.setValue(1, color, 0.0f);
	uniform.submit();

	return program;
}
