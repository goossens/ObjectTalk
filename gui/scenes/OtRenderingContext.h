//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>

#include "bgfx/bgfx.h"
#include "glm/glm.hpp"


//
//	Forward references
//

class OtCameraClass;
typedef std::shared_ptr<OtCameraClass> OtCamera;

class OtSceneClass;
typedef std::shared_ptr<OtSceneClass> OtScene;


//
//	OtRenderingContext class
//

class OtRenderingContextClass;
typedef OtRenderingContextClass* OtRenderingContext;

class OtRenderingContextClass {
public:
	// constructors/destructor
	OtRenderingContextClass();
	OtRenderingContextClass(const OtRenderingContextClass &context);
	~OtRenderingContextClass();

	// clear the context
	void clear();

	// access view ID
	void setView(bgfx::ViewId v) { view = v; }
	bgfx::ViewId getView() { return view; }

	void setViewRect(float x, float y, float w, float h) {
		viewX = x; viewY = y; viewW = w; viewH = h;
		viewAspectRatio = w / h;
	}

	float getViewX() { return viewX; }
	float getViewY() { return viewY; }
	float getViewW() { return viewW; }
	float getViewH() { return viewH; }
	float getViewAspectRatio() { return viewAspectRatio; }

	// access camera
	void setCamera(OtCamera c) { camera = c; }
	OtCamera getCamera() { return camera; }

	// access scene
	void setScene(OtScene s) { scene = s; }
	OtScene getScene() { return scene; }

	// access transform
	void setTransform(const glm::mat4& t) { transform = t; }
	glm::mat4 getTransform() { return transform; }

	// access ambient light color
	void setAmbientLight(const glm::vec3& ambient);
	glm::vec3 getAmbientLight();

	// access directional light
	void setDirectionalLight(const glm::vec3& dir, const glm::vec3& diffuse, const glm::vec3& specular);
	void setPointLight(const glm::vec3& pos, const glm::vec3& diffuse, const glm::vec3& specular);
	OtCamera getCameraFromLightPosition();

	// access flags
	void setReflection(bool r) { reflection = r; }
	bool getReflection() { return reflection; }
	void setRefraction(bool r) { refraction = r; }
	bool getRefraction() { return refraction; }

	// speficy fog parameters
	void setFog(const glm::vec3& color, float near, float far);

	// activate shadow matrix
	void setShadow(bgfx::TextureHandle texture, const glm::mat4& matrix);

	// submit light information to BGFX
	void submit();

private:
	// view identifier
	bgfx::ViewId view;

	// view dimensions
	float viewX;
	float viewY;
	float viewW;
	float viewH;
	float viewAspectRatio;

	// the scene to be rendered
	OtScene scene;

	// camera to render scene with
	OtCamera camera;

	// to support hierarchical scene objects
	glm::mat4 transform = glm::mat4(1.0);

	// to support reflection and refraction renderering
	bool reflection = false;
	bool refraction = false;

	// light slot indexes
	enum {
		cameraPositionSlot = 0,
		ambientLightSlot,
		directionalLightOnOffSlot,
		directionalLightDirectionSlot,
		directionalLightDiffuseSlot,
		directionalLightSpecularSlot,
		fogPropertiesSlot,
		fogColorSlot,
		shadowSlot,
		totalSlots
	};

	// to pass light information to shaders
	glm::vec4 uniforms[totalSlots] = { glm::vec4(0.0) };
	bgfx::TextureHandle shadowmapTexture = BGFX_INVALID_HANDLE;
	glm::mat4 shadowMatrix = glm::mat4(1.0);

	bgfx::UniformHandle lightUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle shadowUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle shadowmapUniform = BGFX_INVALID_HANDLE;
};
