//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"


//
//	Functions
//

glm::vec3 OtColorParseToVec3(const std::string& color);
glm::vec4 OtColorParseToVec4(const std::string& color);
uint32_t OtColorParseToUint32(const std::string& color);
