//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "bgfx/bgfx.h"
#include "glm/glm.hpp"

#include "object3d.h"
#include "fog.h"
#include "light.h"


//
//	OtSceneClass
//

class OtSceneClass;
typedef std::shared_ptr<OtSceneClass> OtScene;

class OtSceneClass : public OtObject3dClass {
public:
	// constructor/destructor
	OtSceneClass();
	~OtSceneClass();

	// ensure specified component is allowed as a child
	void validateChild(OtComponent child);

	// specify the ambient light
	void setAmbientLight(double r, double g, double b) { ambient = glm::vec4(r, g, b, 1.0); }

	// add a point/spot light
	void addLight(OtObject light);

	// specifiy fog for view
	void setFog(OtObject fog);

	// GUI to change lighting parameters
	void renderLightGUI();

	// render content
	void render(int view, const glm::mat4& viewMatrix);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtScene create();

private:
	// possible fog in scene
	OtFog fog;

	// ambient lighting
	glm::vec4 ambient = { 1.0, 1.0, 1.0, 1.0};

	// point/spot lights
	std::vector<OtLight> lights;

	// to pass information to shaders
	bgfx::UniformHandle lightUniform = BGFX_INVALID_HANDLE;
};
