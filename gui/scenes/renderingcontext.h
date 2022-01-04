//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "gui.h"


//
//	Forward references
//

class OtCameraClass;
typedef std::shared_ptr<OtCameraClass> OtCamera;

class OtSceneClass;
typedef std::shared_ptr<OtSceneClass> OtScene;


//
//	Constants
//

#define AMBIENT_LIGHT_SLOTS 1
#define FOG_SLOTS 2

#define DIRECTIONAL_LIGHTS 4
#define SLOTS_PER_LIGHT 4

#define FIXED_LIGHT_SLOTS (AMBIENT_LIGHT_SLOTS + FOG_SLOTS)
#define TOTAL_LIGHT_SLOTS (FIXED_LIGHT_SLOTS + DIRECTIONAL_LIGHTS * SLOTS_PER_LIGHT)


//
//	OtRenderingContext class
//

class OtRenderingContext {
public:
	// constructor/destructor
	OtRenderingContext(int view, float viewAspect, OtScene scene, OtCamera camera);
	~OtRenderingContext();

	// specify the ambient light color
	void setAmbientLight(const glm::vec3& ambient);

	// add a directional light
	void addDirectionalLight(const glm::vec3& dir, const glm::vec3& diffuse, const glm::vec3& specular);

	// add a point light
	void addPointLight(const glm::vec3& pos, const glm::vec3& diffuse, const glm::vec3& specular);

	// speficy fog parameters
	void setFog(const glm::vec3& color, float near, float far);

	// submit light information to BGFX
	void submit();

	// view identifier
	int view;

	// view dimensions
	float viewAspect;

	// the scene to be rendered
	OtScene scene;

	// camera to render scene width
	OtCamera camera;

	// to support hierarchical scene objects
	glm::mat4 transform;

	// to support reflection and refraction renderering
	bool reflection = false;
	bool refraction = false;

private:
	// to pass light information to shaders
	glm::vec4 uniforms[TOTAL_LIGHT_SLOTS];
	int lights;

	bgfx::UniformHandle lightUniform = BGFX_INVALID_HANDLE;
};
