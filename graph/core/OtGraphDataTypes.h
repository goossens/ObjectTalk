//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtTypeList.h"

#include "OtTexture.h"


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
	OtTexture>;

enum {
	OtGraphPinBoolType,
	OtGraphPinIntType,
	OtGraphPinFloatType,
	OtGraphPinVec2Type,
	OtGraphPinVec3Type,
	OtGraphPinVec4Type,
	OtGraphPinTextureType
};

static constexpr const char* OtGraphDataTypeNames[] = {
	"bool",
	"int",
	"float",
	"vec2",
	"vec3",
	"vec4",
	"texture" };
