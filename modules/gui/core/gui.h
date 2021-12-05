//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "bx/bx.h"
#include "bx/easing.h"
#include "bx/math.h"
#include "bx/timer.h"
#include "bx/file.h"

#include "bimg/bimg.h"
#include "bimg/decode.h"

#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bgfx/embedded_shader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/io.hpp"
#include "glm/ext.hpp"

#include "imgui.h"
#include "ImGuiFileDialog.h"

#include "ot/object.h"


//
//	OtGui
//

class OtGuiClass;
typedef std::shared_ptr<OtGuiClass> OtGui;

class OtGuiClass : public OtObjectClass {
public:
	// get type definition
	static OtType getMeta();
};
