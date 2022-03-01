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
#include "debugdraw.h"
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

	// access the rendering phase
	enum {
		shadowmapPhase,
		reflectionPhase,
		refractionPhase,
		mainPhase
	};

	void setPhase(int p) { phase = p; }
	bool inShadowmapPhase() { return phase == shadowmapPhase; }
	bool inReflectionPhase() { return phase == reflectionPhase; }
	bool inRefractionPhase() { return phase == refractionPhase; }
	bool inMainPhase() { return phase == mainPhase; }

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

	// access ambient color
	void setAmbientLight(const glm::vec3& ambient);
	glm::vec3 getAmbientLight();

	// access directional light
	void setLight(const glm::vec3& direction, const glm::vec3& diffuse, const glm::vec3& specular);

	// speficy fog parameters
	void setFog(const glm::vec3& color, float near, float far);

	// activate shadow matrix
	void setShadowMap(bgfx::TextureHandle texture, const glm::mat4& matrix);

	// access handle to debug drawing tool
	void setDebugDraw(DebugDrawEncoder* d) { debugDraw = d; }
	DebugDrawEncoder* getDebugDraw() { return debugDraw; }

	// submit light information to BGFX
	void submit(bool receiveShadow);

private:
	// rendering phase
	int phase;

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

	// light slot indices
	enum {
		cameraPositionSlot = 0,
		ambientSlot,
		lightOnOffSlot,
		lightPositionSlot,
		lightDiffuseSlot,
		lightSpecularSlot,
		fogPropertiesSlot,
		fogColorSlot,
		totalLightSlots
	};

	// shadow slot indices
	enum {
		shadowEnabledSlot = 0,
		shadowMatrixSlot,
		totalShadowSlots = 5
	};

	// to pass light information to shaders
	bgfx::UniformHandle lightUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle shadowUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle shadowmapUniform = BGFX_INVALID_HANDLE;

	glm::vec4 lightUniforms[totalLightSlots] = { glm::vec4(0.0) };
	glm::vec4 shadowUniforms[totalShadowSlots] = { glm::vec4(0.0) };
	bgfx::TextureHandle shadowmapTexture = BGFX_INVALID_HANDLE;
	glm::mat4 shadowMatrix = glm::mat4(1.0);

	// suport for debug drawing
	DebugDrawEncoder* debugDraw;
};
