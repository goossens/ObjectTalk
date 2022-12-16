//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtFunction.h"

#include "OtColor.h"
#include "OtTexturedMaterial.h"


//
//	OtTexturedMaterialClass::init
//

void OtTexturedMaterialClass::init(size_t count, OtObject* parameters) {
	if (count == 3) {
		setTexture(parameters[0]->operator std::string());
		setNormalMap(parameters[2]->operator std::string());
		setScale(parameters[1]->operator float());

	} else if (count == 2) {
		setTexture(parameters[0]->operator std::string());
		setNormalMap(parameters[2]->operator std::string());

	} else if (count == 1) {
		setTexture(parameters[0]->operator std::string());

	} else if (count != 0) {
		OtExcept("[TexturedMaterial] constructor expects up to 3 arguments (not %ld)", count);
	}
}


//
//	OtTexturedMaterialClass::setTexture
//

OtObject OtTexturedMaterialClass::setTexture(const std::string& textureName) {
	if (texture.isValid()) {
		OtExcept("Texture already specified for [TexturedMaterial]");
	}

	texture.loadFromFile(textureName);
	return shared();
}


//
//	OtTexturedMaterialClass::setNormalMap
//

OtObject OtTexturedMaterialClass::setNormalMap(const std::string& normalmapName) {
	if (normalmap.isValid()) {
		OtExcept("NormalMap already specified for [TexturedMaterial]");
	}

	normalmap.loadFromFile(normalmapName);
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
//	OtTexturedMaterialClass::submit
//

void OtTexturedMaterialClass::submit(OtRenderer& renderer, bool instancing) {
	// submit uniform
	uniform.set(0, glm::vec4(ambient, scale));
	uniform.set(1, glm::vec4(diffuse, normalmap.isValid()));
	uniform.set(2, glm::vec4(specular, shininess));
	uniform.submit();

	// set samplers
	textureSampler.submit(1, texture);
	normalmapSampler.submit(2, normalmap);

	// run appropriate shader
	if (instancing) {
		renderer.runShader(instancingShader);

	} else {
		renderer.runShader(shader);
	}
}


//
//	OtTexturedMaterialClass::getMeta
//

OtType OtTexturedMaterialClass::getMeta()
{
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtTexturedMaterialClass>("TexturedMaterial", OtMaterialClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtTexturedMaterialClass::init));

		type->set("setTexture", OtFunctionClass::create(&OtTexturedMaterialClass::setTexture));
		type->set("setNormalMap", OtFunctionClass::create(&OtTexturedMaterialClass::setNormalMap));
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
