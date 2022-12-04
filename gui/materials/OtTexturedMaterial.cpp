//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bgfx/embedded_shader.h"

#include "OtException.h"
#include "OtFunction.h"

#include "OtColor.h"
#include "OtFramework.h"
#include "OtTexturedMaterial.h"
#include "OtTexturedShader.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(OtTexturedVS),
	BGFX_EMBEDDED_SHADER(OtTexturedVSI),
	BGFX_EMBEDDED_SHADER(OtTexturedFS),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtTexturedMaterialClass::init
//

void OtTexturedMaterialClass::init(size_t count, OtObject* parameters) {
	if (count == 2) {
		setTexture(parameters[0]->operator std::string());
		setScale(parameters[1]->operator float());

	} else if (count == 1) {
		setTexture(parameters[0]->operator std::string());

	} else if (count != 0) {
		OtExcept("[TexturedMaterial] constructor expects 0, 1 or 2 arguments (not %ld)", count);
	}
}


//
//	OtTexturedMaterialClass::setTexture
//

OtObject OtTexturedMaterialClass::setTexture(const std::string& textureName) {
	if (bgfx::isValid(texture)) {
		OtExcept("Texture already specified for [TexturedMaterial]");
	}

	texture = OtFrameworkClass::instance()->getTexture(textureName);
	return shared();
}


//
//	OtTexturedMaterialClass::setScale
//

OtObject OtTexturedMaterialClass::setScale(float s) {
	scale = s;
	return shared();
}


//
//	OtTexturedMaterialClass::setAmbient
//

OtObject OtTexturedMaterialClass::setAmbient(const std::string& c) {
	ambient = OtColorParseToVec3(c);
	return shared();
}


//
//	OtTexturedMaterialClass::setDiffuse
//

OtObject OtTexturedMaterialClass::setDiffuse(const std::string& c) {
	diffuse = OtColorParseToVec3(c);
	return shared();
}


//
//	OtTexturedMaterialClass::setSpecular
//

OtObject OtTexturedMaterialClass::setSpecular(const std::string& c) {
	specular = OtColorParseToVec3(c);
	return shared();
}


//
//	OtTexturedMaterialClass::setShininess
//

OtObject OtTexturedMaterialClass::setShininess(float s) {
	shininess = s;
	return shared();
}


//
//	OtTexturedMaterialClass::getNumberOfUniforms
//

size_t OtTexturedMaterialClass::getNumberOfUniforms() {
	return 3;
}


//
//	OtTexturedMaterialClass::getUniforms
//

void OtTexturedMaterialClass::getUniforms(glm::vec4* uniforms) {
	uniforms[0] = glm::vec4(ambient, scale);
	uniforms[1] = glm::vec4(diffuse, 0.0);
	uniforms[2] = glm::vec4(specular, shininess);
}


//
//	OtTexturedMaterialClass::getNumberOfSamplers
//

size_t OtTexturedMaterialClass::getNumberOfSamplers() {
	return 1;
}


//
//	OtTexturedMaterialClass::getSamplerTexture
//

bgfx::TextureHandle OtTexturedMaterialClass::getSamplerTexture(size_t index) {
	if (!bgfx::isValid(texture)) {
		OtExcept("[Texture] not set for [TexturedMaterial]");
	}

	return texture;
}


//
//	OtTexturedMaterialClass::createShader
//

bgfx::ProgramHandle OtTexturedMaterialClass::createShader() {
	return OtFrameworkClass::instance()->getProgram(embeddedShaders, "OtTexturedVS", "OtTexturedFS");
}


//
//	OtTexturedMaterialClass::createInstancingShader
//

bgfx::ProgramHandle OtTexturedMaterialClass::createInstancingShader() {
	return OtFrameworkClass::instance()->getProgram(embeddedShaders, "OtTexturedVSI", "OtTexturedFS");
}


//
//	OtTexturedMaterialClass::getMeta
//

OtType OtTexturedMaterialClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtTexturedMaterialClass>("TexturedMaterial", OtMaterialClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtTexturedMaterialClass::init));

		type->set("setTexture", OtFunctionClass::create(&OtTexturedMaterialClass::setTexture));
		type->set("setScale", OtFunctionClass::create(&OtTexturedMaterialClass::setScale));

		type->set("setAmbient", OtFunctionClass::create(&OtTexturedMaterialClass::setAmbient));
		type->set("setDiffuse", OtFunctionClass::create(&OtTexturedMaterialClass::setDiffuse));
		type->set("setSpecular", OtFunctionClass::create(&OtTexturedMaterialClass::setSpecular));
		type->set("setShininess", OtFunctionClass::create(&OtTexturedMaterialClass::setShininess));
	}

	return type;
}


//
//	OtTexturedMaterialClass::create
//

OtTexturedMaterial OtTexturedMaterialClass::create() {
	OtTexturedMaterial material = std::make_shared<OtTexturedMaterialClass>();
	material->setType(getMeta());
	return material;
}
