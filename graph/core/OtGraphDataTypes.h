//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtTypeList.h"


//
//	Data types
//

using OtGraphDataTypes = OtTypeList<
	bool,
	int,
	float,
	glm::vec2,
	glm::vec3,
	glm::vec4,
	uint16_t>;

static constexpr const char* OtGraphDataTypeNames[] = {
	"bool",
	"int",
	"float",
	"vec2",
	"vec3",
	"vec4",
	"image" };
