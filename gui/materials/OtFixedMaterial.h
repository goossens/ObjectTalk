//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
#include "OtUniformVec4.h"


//
//	OtMaterial
//

class OtFixedMaterialClass;
using OtFixedMaterial = OtObjectPointer<OtFixedMaterialClass>;

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
	void submit(OtRenderer& renderer, bool wireframe, bool instancing) override;

	// get type definition
	static OtType getMeta();

private:
	// properties
	glm::vec3 color = { 1.0, 1.0, 1.0 };
	float opacity = 1.0;

	// GPU assets
	OtUniformVec4 uniform = OtUniformVec4("u_material", 1);

	OtShader regularShader = OtShader("OtFixedVS", "OtFixedFS");
	OtShader instancingShader = OtShader("OtFixedVSI", "OtFixedFS");
	OtShader shadowShader = OtShader("OtShadowVS", "OtShadowFS");
	OtShader shadowInstancingShader = OtShader("OtShadowVSI", "OtShadowFS");
};
