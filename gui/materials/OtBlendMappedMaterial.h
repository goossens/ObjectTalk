//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "bgfx/bgfx.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "imgui.h"

#include "OtController.h"
#include "OtMaterial.h"


//
//	OtMaterial
//

class OtBlendMappedMaterialClass;
typedef std::shared_ptr<OtBlendMappedMaterialClass> OtBlendMappedMaterial;

class OtBlendMappedMaterialClass : public OtMaterialClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// set properties
	OtObject setTextures(const std::string& bm, const std::string& tn, const std::string& tr, const std::string& tg, const std::string& tb);
	OtObject setScale(float scale);

	OtObject setAmbient(const std::string& color);
	OtObject setDiffuse(const std::string& color);
	OtObject setSpecular(const std::string& color);
	OtObject setShininess(float shininess);

	void setAmbientRGB(float r, float g, float b) { ambient = glm::vec3(r, g, b); }
	void setDiffuseRGB(float r, float g, float b) { diffuse = glm::vec3(r, g, b); }
	void setSpecularRGB(float r, float g, float b) { specular = glm::vec3(r, g, b); }

	// GUI to change properties
	void renderGUI();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtBlendMappedMaterial create();

private:
	// properties
	bgfx::TextureHandle blendmap = BGFX_INVALID_HANDLE;
	bgfx::TextureHandle textureN = BGFX_INVALID_HANDLE;
	bgfx::TextureHandle textureR = BGFX_INVALID_HANDLE;
	bgfx::TextureHandle textureG = BGFX_INVALID_HANDLE;
	bgfx::TextureHandle textureB = BGFX_INVALID_HANDLE;
	float scale = 1.0;

	glm::vec3 ambient = { 0.4, 0.4, 0.4 };
	glm::vec3 diffuse = { 0.6, 0.6, 0.6 };
	glm::vec3 specular = { 0.4, 0.4, 0.4 };
	float shininess = 20;

	// uniform and shader functions
	size_t getNumberOfUniforms() override;
	void getUniforms(glm::vec4* uniforms) override;

	size_t getNumberOfSamplers() override;
	bgfx::TextureHandle getSamplerTexture(size_t index) override;

	bgfx::ProgramHandle createShader() override;
	bgfx::ProgramHandle createInstancingShader() override;
};


//
//	Controller widget
//

OT_CONTROLLER(BlendMappedMaterial)
