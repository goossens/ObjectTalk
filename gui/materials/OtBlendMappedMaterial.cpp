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
#include "OtBlendMappedMaterial.h"
#include "OtBlendMappedShader.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(OtBlendMappedVS),
	BGFX_EMBEDDED_SHADER(OtBlendMappedVSI),
	BGFX_EMBEDDED_SHADER(OtBlendMappedFS),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtBlendMappedMaterialClass::init
//

void OtBlendMappedMaterialClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 6:
			setScale(parameters[5]->operator float());

		case 5:
			setTextures(
				parameters[0]->operator std::string(),
				parameters[1]->operator std::string(),
				parameters[2]->operator std::string(),
				parameters[3]->operator std::string(),
				parameters[4]->operator std::string());

			break;

		case 0:
			break;

		default:
			OtExcept("[BlendMappedMaterial] constructor expects 0, 5 or 6 arguments (not %ld)", count);
	}
}


//
//	OtBlendMappedMaterialClass::setMaps
//

OtObject OtBlendMappedMaterialClass::setTextures(const std::string& bm, const std::string& tn, const std::string& tr, const std::string& tg, const std::string& tb) {
	if (bgfx::isValid(blendmap)) {
		OtExcept("Textures already specified for [BlendMappedMaterial]");
	}

	auto framework = OtFrameworkClass::instance();
	blendmap = framework->getTexture(bm, true);
	textureN = framework->getTexture(tn, true);
	textureR = framework->getTexture(tr, true);
	textureG = framework->getTexture(tg, true);
	textureB = framework->getTexture(tb, true);
	return shared();
}


//
//	OtBlendMappedMaterialClass::setScale
//

OtObject OtBlendMappedMaterialClass::setScale(float s) {
	scale = s;
	return shared();
}


//
//	OtBlendMappedMaterialClass::setAmbient
//

OtObject OtBlendMappedMaterialClass::setAmbient(const std::string& c) {
	ambient = OtColorParseToVec3(c);
	return shared();
}


//
//	OtBlendMappedMaterialClass::setDiffuse
//

OtObject OtBlendMappedMaterialClass::setDiffuse(const std::string& c) {
	diffuse = OtColorParseToVec3(c);
	return shared();
}


//
//	OtBlendMappedMaterialClass::setSpecular
//

OtObject OtBlendMappedMaterialClass::setSpecular(const std::string& c) {
	specular = OtColorParseToVec3(c);
	return shared();
}


//
//	OtBlendMappedMaterialClass::setShininess
//

OtObject OtBlendMappedMaterialClass::setShininess(float s) {
	shininess = s;
	return shared();
}


//
//	OtBlendMappedMaterialClass::getNumberOfUniforms
//

size_t OtBlendMappedMaterialClass::getNumberOfUniforms() {
	return 3;
}


//
//	OtBlendMappedMaterialClass::getUniforms
//

void OtBlendMappedMaterialClass::getUniforms(glm::vec4* uniforms) {
	uniforms[0] = glm::vec4(ambient, scale);
	uniforms[1] = glm::vec4(diffuse, 0.0);
	uniforms[2] = glm::vec4(specular, shininess);
}


//
//	OtBlendMappedMaterialClass::getNumberOfSamplers
//

size_t OtBlendMappedMaterialClass::getNumberOfSamplers() {
	return 5;
}


//
//	OtBlendMappedMaterialClass::getSamplerTexture
//

bgfx::TextureHandle OtBlendMappedMaterialClass::getSamplerTexture(size_t index) {
	if (!bgfx::isValid(blendmap)) {
		OtExcept("[Textures] not set for [BlendMappedMaterial]");
	}

	if (index == 1) {
		return blendmap;

	} else if (index == 2) {
		return textureN;

	} else if (index == 3) {
		return textureR;

	} else if (index == 4) {
		return textureG;

	} else {
		return textureB;
	}
}


//
//	OtBlendMappedMaterialClass::createShader
//

bgfx::ProgramHandle OtBlendMappedMaterialClass::createShader() {
	return OtFrameworkClass::instance()->getProgram(embeddedShaders, "OtBlendMappedVS", "OtBlendMappedFS");
}


//
//	OtBlendMappedMaterialClass::createInstancingShader
//

bgfx::ProgramHandle OtBlendMappedMaterialClass::createInstancingShader() {
	return OtFrameworkClass::instance()->getProgram(embeddedShaders, "OtBlendMappedVSI", "OtBlendMappedFS");
}


//
//	OtBlendMappedMaterialClass::renderGUI
//

void OtBlendMappedMaterialClass::renderGUI() {
	ImGui::SliderFloat("Scale", &scale, 1.0, 100.0);
	ImGui::ColorEdit3("Ambient", glm::value_ptr(ambient));
	ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse));
	ImGui::ColorEdit3("Specular", glm::value_ptr(specular));
	ImGui::SliderFloat("Shininess", &shininess, 0.0, 1.0);
}


//
//	OtBlendMappedMaterialClass::getMeta
//

OtType OtBlendMappedMaterialClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtBlendMappedMaterialClass>("BlendMappedMaterial", OtMaterialClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtBlendMappedMaterialClass::init));

		type->set("setTextures", OtFunctionClass::create(&OtBlendMappedMaterialClass::setTextures));
		type->set("setScale", OtFunctionClass::create(&OtBlendMappedMaterialClass::setScale));

		type->set("setAmbient", OtFunctionClass::create(&OtBlendMappedMaterialClass::setAmbient));
		type->set("setDiffuse", OtFunctionClass::create(&OtBlendMappedMaterialClass::setDiffuse));
		type->set("setSpecular", OtFunctionClass::create(&OtBlendMappedMaterialClass::setSpecular));
		type->set("setShininess", OtFunctionClass::create(&OtBlendMappedMaterialClass::setShininess));
	}

	return type;
}


//
//	OtBlendMappedMaterialClass::create
//

OtBlendMappedMaterial OtBlendMappedMaterialClass::create() {
	OtBlendMappedMaterial material = std::make_shared<OtBlendMappedMaterialClass>();
	material->setType(getMeta());
	return material;
}
