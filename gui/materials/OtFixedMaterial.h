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

class OtFixedMaterialClass;
typedef std::shared_ptr<OtFixedMaterialClass> OtFixedMaterial;

class OtFixedMaterialClass : public OtMaterialClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// set properties
	OtObject setColor(const std::string& color);
	OtObject setColorRGB(float r, float g, float b);
	OtObject setOpacity(float opacity);

	void setColorVector(const glm::vec3& c) { color = c; }

	// submit to GPU
	void submit(OtRenderer& renderer, bool instancing) override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtFixedMaterial create();

private:
	// properties
	glm::vec3 color = { 1.0, 1.0, 1.0 };
	float opacity = 1.0;

	// GPU assets
	OtUniform uniform = OtUniform("u_material", 1);

	OtShader shader = OtShader("OtFixedVS", "OtFixedFS");
	OtShader instancingShader = OtShader("OtFixedVSI", "OtFixedFS");
};
