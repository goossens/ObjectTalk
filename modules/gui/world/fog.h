//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/ext.hpp"
#include "glm/glm.hpp"

#include "bgfx/bgfx.h"

#include "gui.h"


//
//	OtFogClass
//

class OtFogClass;
typedef std::shared_ptr<OtFogClass> OtFog;

class OtFogClass : public OtGuiClass {
	friend class OtViewClass;

public:
	// constructor/destructor
	OtFogClass();
	~OtFogClass();

	// initialize fog
	OtObject init(size_t count, OtObject* parameters);

	// GUI to change fog parameters
	void renderGUI();

	// submit data to BGFX
	void submit();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtFog create();

private:
	// fog properties
	bool active = false;
	glm::vec4 color = glm::vec4(1.0);
	float near = 0;
	float far = 100;

	// to pass information to shaders
	bgfx::UniformHandle fogUniform = BGFX_INVALID_HANDLE;
};
