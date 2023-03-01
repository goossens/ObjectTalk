//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtFunction.h"

#include "OtColor.h"
#include "OtFramework.h"
#include "OtBlendMappedMaterial.h"


//
//	OtBlendMappedMaterialClass::init
//

void OtBlendMappedMaterialClass::init(size_t count, OtObject *parameters) {
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
	if (blendmap.isValid()) {
		OtExcept("Textures already specified for [BlendMappedMaterial]");
	}

	blendmap.loadFromFile(bm, true);
	textureNone.loadFromFile(tn, true);
	textureRed.loadFromFile(tr, true);
	textureGreen.loadFromFile(tg, true);
	textureBlue.loadFromFile(tb, true);
	return OtObject(this);
}


//
//	OtBlendMappedMaterialClass::setScale
//

OtObject OtBlendMappedMaterialClass::setScale(float s) {
	scale = s;
	return OtObject(this);
}


//
//	OtBlendMappedMaterialClass::setAmbient
//

OtObject OtBlendMappedMaterialClass::setAmbient(const std::string& c) {
	ambient = OtColorParseToVec3(c);
	return OtObject(this);
}


//
//	OtBlendMappedMaterialClass::setDiffuse
//

OtObject OtBlendMappedMaterialClass::setDiffuse(const std::string& c) {
	diffuse = OtColorParseToVec3(c);
	return OtObject(this);
}


//
//	OtBlendMappedMaterialClass::setSpecular
//

OtObject OtBlendMappedMaterialClass::setSpecular(const std::string& c) {
	specular = OtColorParseToVec3(c);
	return OtObject(this);
}


//
//	OtBlendMappedMaterialClass::setShininess
//

OtObject OtBlendMappedMaterialClass::setShininess(float s) {
	shininess = s;
	return OtObject(this);
}


//
//	OtBlendMappedMaterialClass::submit
//

void OtBlendMappedMaterialClass::submit(OtRenderer& renderer, bool wireframe, bool instancing) {
	if (renderer.inShadowmapPass()) {
		if (instancing) {
			renderer.runShader(shadowInstancingShader);

		} else {
			renderer.runShader(shadowShader);
		}

	} else {
		// submit uniform
		uniform.set(0, glm::vec4(ambient, scale));
		uniform.set(1, glm::vec4(diffuse, 0.0));
		uniform.set(2, glm::vec4(specular, shininess));
		uniform.submit();

		// set samplers
		blendmapSampler.submit(1, blendmap);
		textureNoneSampler.submit(2, textureNone);
		textureRedSampler.submit(3, textureRed);
		textureGreenSampler.submit(4, textureGreen);
		textureBlueSampler.submit(5, textureBlue);

		// set rendering state
		renderer.setState(wireframe, frontside, backside, transparent);

		// run appropriate shader
		if (instancing) {
			renderer.runShader(instancingShader);

		} else {
			renderer.runShader(regularShader);
		}
	}
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
		type->set("__init__", OtFunction::create(&OtBlendMappedMaterialClass::init));

		type->set("setTextures", OtFunction::create(&OtBlendMappedMaterialClass::setTextures));
		type->set("setScale", OtFunction::create(&OtBlendMappedMaterialClass::setScale));

		type->set("setAmbient", OtFunction::create(&OtBlendMappedMaterialClass::setAmbient));
		type->set("setDiffuse", OtFunction::create(&OtBlendMappedMaterialClass::setDiffuse));
		type->set("setSpecular", OtFunction::create(&OtBlendMappedMaterialClass::setSpecular));
		type->set("setShininess", OtFunction::create(&OtBlendMappedMaterialClass::setShininess));
	}

	return type;
}
