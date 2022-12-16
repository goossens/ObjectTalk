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
#include <vector>

#include "glm/glm.hpp"

#include "OtFrameBuffer.h"
#include "OtPass.h"
#include "OtSampler.h"
#include "OtShader.h"
#include "OtUniform.h"
#include "OtVertex.h"


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
//	OtRenderer class
//

class OtRenderer {
public:
	// constructor
	OtRenderer();

	// run various rendering passes
	void runLightingPass(OtScene scene, OtCamera camera, float x, float y, float w, float h);
	void runShadowPass(OtPass pass, OtScene scene, OtCamera camera, OtFrameBuffer& framebuffer);
	void runReflectionPass(OtPass pass, OtScene scene, OtCamera camera, OtFrameBuffer& framebuffer);

	// validate pass
	bool inShadowmapPass() { return renderingPass == shadowmapPass; }
	bool inReflectionPass() { return renderingPass == reflectionPass; }
	bool inRefractionPass() { return renderingPass == refractionPass; }
	bool inLightingPass() { return renderingPass == lightingPass; }

	// access view rectangle
	float getViewX() { return viewX; }
	float getViewY() { return viewY; }
	float getViewW() { return viewW; }
	float getViewH() { return viewH; }

	// update rendering state
	void setTransform(const glm::mat4& matrix);
	void setState(uint64_t state);
	void setDefaultState();
	void setInstanceData(void* data, size_t count, size_t size);

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

	// copy light properties from another renderer
	void copyLightProperties(OtRenderer& renderer);

	// activate shadow matrix
	void setShadowMap(OtFrameBuffer& framebuffer, const glm::mat4& matrix);

	// submit rendering context to GPU
	void submit(bool receiveShadow);

	// run a shader program
	void runShader(OtShader& shader) { pass.runShader(shader); }

	// debug support
	void debugSetColor(uint32_t color) {debugColor = color; }
	void debugMoveTo(const glm::vec3& point) { debugPoint = point; }
	void debugLineTo(const glm::vec3& point);
	void debugAxis(float scale);

private:
	// track rendering pass
	enum RenderingPass{
		shadowmapPass,
		reflectionPass,
		refractionPass,
		lightingPass
	};

	RenderingPass renderingPass;

	// pass settings
	OtPass pass;

	// view dimensions
	float viewX;
	float viewY;
	float viewW;
	float viewH;

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
	OtFrameBuffer* shadowFrameBuffer = nullptr;
	glm::mat4 shadowMatrix = glm::mat4(1.0);

	// to pass light information to shaders
	OtUniform lightUniform;
	OtSampler shadowSampler = OtSampler("s_shadowmap");

	// support for debug drawing
	uint32_t debugColor = 0;
	glm::vec3 debugPoint = glm::vec3(0.0);
	std::vector<OtVertexPosCol> debugVertices;
	std::vector<uint32_t> debugIndices;
	void debugRender();
};
