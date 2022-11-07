//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"
#include "glm/glm.hpp"

#include "OtCamera.h"
#include "OtController.h"
#include "OtSceneObject.h"


//
//	OtLightClass
//

class OtLightClass;
typedef std::shared_ptr<OtLightClass> OtLight;

class OtLightClass : public OtSceneObjectClass {
public:
	// constructor/destructor
	OtLightClass();
	~OtLightClass();

	// update attributes
	OtObject setDirection(float x, float y, float z);
	OtObject setDiffuse(const std::string& color) ;
	OtObject setSpecular(const std::string& color);

	void setDirectionVector(const glm::vec3& d) { direction = d; }
	void setDiffuseVector(const glm::vec3& c) { diffuse = c; }
	void setSpecularVector(const glm::vec3& c) { specular = c; }

	// enable shadow casting
	OtObject castShadow(float width, float near, float far, bool debug);
	void toggleShadow(bool flag) { shadow = flag; }

	// update state
	void update(OtRenderingContext context) override;

	// render in BGFX
	void render(OtRenderingContext context) override;
	void renderShadowmap(OtRenderingContext context);

	// GUI to change light properties
	void renderGUI();
	void renderShadowCameraGUI();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtLight create();

private:
	// light properties
	glm::vec3 direction = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 diffuse = glm::vec3(1.0);
	glm::vec3 specular = glm::vec3(1.0);

	// shadowmap
	bool shadow = false;
	bgfx::TextureHandle shadowmapTexture = BGFX_INVALID_HANDLE;
	bgfx::FrameBufferHandle shadowmapFrameBuffer = BGFX_INVALID_HANDLE;
	OtCamera shadowCamera;
	bgfx::ViewId shadowView;
};


//
//	Controller widget
//

OT_CONTROLLER(Light)
