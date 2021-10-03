//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "gui.h"
#include "texture.h"
#include "blendmap.h"


//
//	OtMaterial
//

class OtMaterialClass;
typedef std::shared_ptr<OtMaterialClass> OtMaterial;

class OtMaterialClass : public OtGuiClass {
public:
	// material types
	enum {
		FIXED,
		COLORED,
		TEXTURED,
		BLENDMAPPED
	};

	// constructor/destructor
	OtMaterialClass();
	~OtMaterialClass();

	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// set properties
	OtObject setFixed(const std::string& color);

	OtObject setMaterial(const std::string& name);
	OtObject setColor(const std::string& color);

	OtObject setAmbient(const std::string c);
	OtObject setDiffuse(const std::string c) ;
	OtObject setSpecular(const std::string c);
	OtObject setShininess(float s);
	OtObject setTransparency(float t);

	OtObject setTexture(OtObject texture);
	OtObject setBlendMap(OtObject blendmap);

	// set UV transformation
	OtObject setUvTransform(
		float offsetX, float offsetY,
		float repeatX, float repeatY,
		float rotation,
		float centerX, float centerY);

	// submit shader data to BGFX
	void submit();

	// get material type
	int getType();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtMaterial create();

	// get list of detault materials
	static OtObject getDefaultMaterials();

private:
	// material properties
	bool fixed = false;

	glm::vec3 ambient = { 0.4, 0.4, 0.4 };
	glm::vec3 diffuse = { 0.6, 0.6, 0.6 };
	glm::vec3 specular = { 0.4, 0.4, 0.4 };
	float shininess = 20;
	float transparency = 1.0;

	glm::mat3 uvTransform = glm::mat3(1.0);

	// material information
	bgfx::UniformHandle materialUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle transformUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniform = BGFX_INVALID_HANDLE;

	OtTexture texture;
	OtBlendMap blendmap;
};
