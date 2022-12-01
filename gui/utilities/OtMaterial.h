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

#include "OtBlendMap.h"
#include "OtGui.h"


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
		NORMALLED,
		BLENDMAPPED
	};

	// constructor/destructor
	OtMaterialClass();
	~OtMaterialClass();

	// initialize
	void init(size_t count, OtObject* parameters);

	// set properties
	OtObject setFixed(const std::string& color);

	OtObject setMaterial(const std::string& name);
	OtObject setColor(const std::string& color);

	OtObject setAmbient(const std::string& c);
	OtObject setDiffuse(const std::string& c) ;
	OtObject setSpecular(const std::string& c);
	OtObject setShininess(float s);
	OtObject setOpacity(float t);

	OtObject setTexture(const std::string& texture);
	OtObject setMipMap(const std::string& texture);
	OtObject setNormals(const std::string& texture, const std::string& normals);
	OtObject setBlendMap(OtObject blendmap);

	void setColorVector(const glm::vec3& c) { color = c; }
	void setAmbientVector(const glm::vec3& c) { ambient = c; }
	void setDiffuseVector(const glm::vec3& c) { diffuse = c; }
	void setSpecularVector(const glm::vec3& c) { specular = c; }

	void setColorRGB(float r, float g, float b) { color = glm::vec3(r, g, b); }
	void setAmbientRGB(float r, float g, float b) { ambient = glm::vec3(r, g, b); }
	void setDiffuseRGB(float r, float g, float b) { diffuse = glm::vec3(r, g, b); }
	void setSpecularRGB(float r, float g, float b) { specular = glm::vec3(r, g, b); }

	// set UV transformation
	OtObject setUvTransform(
		float offsetX, float offsetY,
		float repeatX, float repeatY,
		float rotation,
		float centerX, float centerY);

	// determines which side of faces will be rendered
	OtObject setFrontSide() { frontside = true; backside = false; return shared(); }
	OtObject setBackSide() { frontside = false; backside = true; return shared(); }
	OtObject setDoubleSided() { frontside = true; backside = true; return shared(); }
	bool getFrontSide() { return frontside; }
	bool getBackSide() { return backside; }

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

	glm::vec3 color = { 1.0, 1.0, 1.0 };
	glm::vec3 ambient = { 0.4, 0.4, 0.4 };
	glm::vec3 diffuse = { 0.6, 0.6, 0.6 };
	glm::vec3 specular = { 0.4, 0.4, 0.4 };
	float shininess = 20;
	float opacity = 1.0;

	glm::mat3 uvTransform = glm::mat3(1.0);

	bool frontside = true;
	bool backside = false;

	// material information
	bgfx::UniformHandle materialUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle transformUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle normalsUniform = BGFX_INVALID_HANDLE;

	bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;
	bgfx::TextureHandle normals = BGFX_INVALID_HANDLE;
	OtBlendMap blendmap;
};
