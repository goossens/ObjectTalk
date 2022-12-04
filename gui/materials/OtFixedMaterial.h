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

#include "OtMaterial.h"


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

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtFixedMaterial create();

private:
	// properties
	glm::vec3 color = { 1.0, 1.0, 1.0 };
	float opacity = 1.0;

	// uniform and shader functions
	size_t getNumberOfUniforms() override;
	void getUniforms(glm::vec4* uniforms) override;

	bgfx::ProgramHandle createShader() override;
	bgfx::ProgramHandle createInstancingShader() override;
};
