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
//	Constants
//

inline constexpr size_t maxPointSpotLights = 4;


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
	// constructor
	OtRenderingContextClass();

	// access the rendering pass
	enum {
		shadowmapPass,
		reflectionPass,
		refractionPass,
		lightingPass
	};

	void setPass(int p) { pass = p; }
	bool inShadowmapPass() { return pass == shadowmapPass; }
	bool inReflectionPass() { return pass == reflectionPass; }
	bool inRefractionPass() { return pass == refractionPass; }
	bool inLightingPass() { return pass == lightingPass; }

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

	// add ambient color
	void addAmbientLight(const glm::vec3& ambient);

	// set directional light
	void setDirectionalLight(const glm::vec3& direction, const glm::vec3& color);

	// add a point light
	void addPointLight(const glm::vec3& position, const glm::vec3& color, const glm::vec3& attenuation);

	// add a spot light
	void addSpotLight(
		const glm::vec3& position, const glm::vec3& direction,
		const glm::vec3& color, const glm::vec3& attenuation,
		float coneInner, float coneOuter);

	// specify fog parameters
	void setFog(const glm::vec3& color, float near, float far);

	// copy light properties from another context
	void copyLightProperties(OtRenderingContext context);

	// activate shadow matrix
	void setShadowMap(bgfx::TextureHandle texture, const glm::mat4& matrix);

	// access handle to debug drawing tool
	void setDebugDraw(DebugDrawEncoder* d) { debugDraw = d; }
	DebugDrawEncoder* getDebugDraw() { return debugDraw; }

	// submit to GPU
	void submit(bool receiveShadow);

private:
	// rendering pass
	int pass;

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

	// light properties
	glm::vec3 ambientLightColor = glm::vec3(0.0);

	bool directionalLightEnabled = false;
	glm::vec3 directionalLightDirection = glm::vec3(0.0);
	glm::vec3 directionalLightColor = glm::vec3(0.0);

	enum LightType {
		unusedLight = 0,
		pointLight,
		spotLight
	};

	struct PointSpotLight {
		LightType type = unusedLight;
		glm::vec3 position = glm::vec3(0.0);
		glm::vec3 color = glm::vec3(0.0);
		glm::vec3 attenuation = glm::vec3(0.0);
		glm::vec3 coneDirection = glm::vec3(0.0);
		float coneInner = 0.0;
		float coneOuter = 0.0;
	};

	PointSpotLight pointSpotLight[maxPointSpotLights];
	size_t lightCount = 0;

	bool fogEnabled = false;
	glm::vec3 fogColor = glm::vec3(1.0);
	float fogNear = 0.0;
	float fogFar = 0.0;

	bool shadowEnabled = false;
	glm::mat4 shadowMatrix = glm::mat4(1.0);
	bgfx::TextureHandle shadowmapTexture = BGFX_INVALID_HANDLE;

	// to pass light information to shaders
	size_t uniformCount;
	bgfx::UniformHandle lightUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle shadowmapUniform = BGFX_INVALID_HANDLE;

	// suport for debug drawing
	DebugDrawEncoder* debugDraw;
};
