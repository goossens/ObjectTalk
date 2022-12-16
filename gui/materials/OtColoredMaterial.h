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
#include "OtShader.h"
#include "OtUniform.h"


//
//	OtMaterial
//

class OtColoredMaterialClass;
typedef std::shared_ptr<OtColoredMaterialClass> OtColoredMaterial;

class OtColoredMaterialClass : public OtMaterialClass {
public:

	// initialize
	void init(size_t count, OtObject* parameters);

	// set properties
	OtObject setColor(const std::string& color);
	OtObject setColorRGB(float r, float g, float b);
	OtObject setOpacity(float opacity);

	OtObject setAmbient(const std::string& color);
	OtObject setDiffuse(const std::string& color);
	OtObject setSpecular(const std::string& color);
	OtObject setShininess(float shininess);

	void setColorVector(const glm::vec3& c) { color = c; }
	void setAmbientRGB(float r, float g, float b) { ambient = glm::vec3(r, g, b); }
	void setDiffuseRGB(float r, float g, float b) { diffuse = glm::vec3(r, g, b); }
	void setSpecularRGB(float r, float g, float b) { specular = glm::vec3(r, g, b); }

	// submit to GPU
	void submit(OtRenderer& renderer, bool instancing) override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtColoredMaterial create();

	// get list of detault materials
	static OtObject getDefaultMaterialNames();

private:
	// properties
	glm::vec3 color = { 1.0, 1.0, 1.0 };
	float opacity = 1.0;

	glm::vec3 ambient = { 0.4, 0.4, 0.4 };
	glm::vec3 diffuse = { 0.6, 0.6, 0.6 };
	glm::vec3 specular = { 0.4, 0.4, 0.4 };
	float shininess = 20;

	// GPU assets
	OtUniform uniform = OtUniform("u_material", 4);

	OtShader shader = OtShader("OtColoredVS", "OtColoredFS");
	OtShader instancingShader = OtShader("OtColoredVSI", "OtColoredFS");
};
