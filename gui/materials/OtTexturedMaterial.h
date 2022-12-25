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

#include "glm/glm.hpp"

#include "OtMaterial.h"
#include "OtSampler.h"
#include "OtShader.h"
#include "OtTexture.h"
#include "OtUniformVec4.h"


//
//	OtMaterial
//

class OtTexturedMaterialClass;
typedef std::shared_ptr<OtTexturedMaterialClass> OtTexturedMaterial;

class OtTexturedMaterialClass : public OtMaterialClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// set properties
	OtObject setTexture(const std::string& textureName);
	OtObject setNormalMap(const std::string& normalmapName);
	OtObject setScale(float scale);
	OtObject setTransparent(bool tranparent);

	OtObject setAmbient(const std::string& color);
	OtObject setDiffuse(const std::string& color);
	OtObject setSpecular(const std::string& color);
	OtObject setShininess(float shininess);

	void setAmbientRGB(float r, float g, float b) { ambient = glm::vec3(r, g, b); }
	void setDiffuseRGB(float r, float g, float b) { diffuse = glm::vec3(r, g, b); }
	void setSpecularRGB(float r, float g, float b) { specular = glm::vec3(r, g, b); }

	// submit to GPU
	void submit(OtRenderer& renderer, bool wireframe, bool instancing) override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtTexturedMaterial create();

private:
	// properties
	OtTexture texture;
	OtTexture normalmap;
	float scale = 1.0;
	bool transparent = false;

	glm::vec3 ambient = { 0.4, 0.4, 0.4 };
	glm::vec3 diffuse = { 0.6, 0.6, 0.6 };
	glm::vec3 specular = { 0.4, 0.4, 0.4 };
	float shininess = 20;

	// GPU assets
	OtUniformVec4 materialUniform = OtUniformVec4("u_material", 3);
	OtUniformVec4 scaleUniform = OtUniformVec4("u_scale", 1);

	OtSampler textureSampler = OtSampler("s_texture");
	OtSampler normalmapSampler = OtSampler("s_normalmap");

	OtShader regularShader = OtShader("OtTexturedVS", "OtTexturedFS");
	OtShader instancingShader = OtShader("OtTexturedVSI", "OtTexturedFS");

	OtShader shadowShader = OtShader("OtShadowVS", "OtShadowFS");
	OtShader transparentShadowShader = OtShader("OtTransparentShadowVS", "OtTransparentShadowFS");

	OtShader shadowInstancingShader = OtShader("OtShadowVSI", "OtShadowFS");
	OtShader transparentShadowInstancingShader = OtShader("OtTransparentShadowVSI", "OtTransparentShadowFS");
};
