//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <string>

#include "glm/glm.hpp"


//
//	OtColorParser
//

class OtColorParser {
public:
	static glm::vec3 toVec3(const std::string& color);
	static glm::vec4 toVec4(const std::string& color);
	static uint32_t toUint32(const std::string& color);
};
