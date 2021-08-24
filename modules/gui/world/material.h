//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"
#include "bgfx/bgfx.h"
#include "bimg/decode.h"
#include "tiny_obj_loader.h"

#include "gui.h"


//
//	OtMaterial
//

class OtMaterialClass;
typedef std::shared_ptr<OtMaterialClass> OtMaterial;

class OtMaterialClass : public OtGuiClass {
public:
	// constructor/destructor
	OtMaterialClass();
	~OtMaterialClass();

	// set properties
	OtObject setMaterial(const std::string& name);
	OtObject setColorRGB(double r, double g, double b);
	OtObject setColorCSS(const std::string& color);
	OtObject setTexture(const std::string& file);
	OtObject setVertex();
	OtObject setAmbientRGB(double r, double g, double b);
	OtObject setAmbientCSS(const std::string c);
	OtObject setDiffuseRGB(double r, double g, double b);
	OtObject setDiffuseCSS(const std::string c) ;
	OtObject setSpecularRGB(double r, double g, double b);
	OtObject setSpecularCSS(const std::string c);
	OtObject setShininess(double s);
	OtObject setTransparency(double t);

	// set UV transformation
	OtObject setUvTransform(
		double offsetX, double offsetY,
		double repeatX, double repeatY,
		double rotation,
		double centerX, double centerY);

	// submit shader data to BGFX
	virtual void submit(int view);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtMaterial create();
	static OtMaterial create(const tinyobj::material_t& material);

	// get list of detault materials
	static OtObject getDefaultMaterials();

private:
	// material properties
	bool vertex = false;
	glm::vec3 ambient = { 0.4, 0.4, 0.4 };
	glm::vec3 diffuse = { 0.6, 0.6, 0.6 };
	glm::vec3 specular = { 0.4, 0.4, 0.4 };
	double shininess = 20;
	double transparency = 1.0;

	glm::mat3 uvTransform = glm::mat3(1.0);

	// material information
	bgfx::UniformHandle materialUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle transformUniform = BGFX_INVALID_HANDLE;

	// our texture (if required)
	bgfx::UniformHandle textureUniform = BGFX_INVALID_HANDLE;
	bimg::ImageContainer* image = nullptr;
	bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;
	bgfx::TextureHandle dummy = BGFX_INVALID_HANDLE;
};
