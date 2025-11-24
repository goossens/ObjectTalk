//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "imgui.h"

#include "OtProjector.h"


//
//	OtMapLayer class
//

class OtMapLayer {
public:
	virtual ~OtMapLayer() {}
	virtual void render(ImDrawList* drawlist, OtProjector& projector) = 0;
};
