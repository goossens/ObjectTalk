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
	return OtObject(this);
}


//
//	OtTexturedMaterialClass::setNormalMap
//

OtObject OtTexturedMaterialClass::setNormalMap(const std::string& normalmapName) {
	if (normalmap.isValid()) {
		OtExcept("NormalMap already specified for [TexturedMaterial]");
	}

	normalmap.loadFromFile(normalmapName);
	return OtObject(this);
}


//
//	OtTexturedMaterialClass::setScale
//

OtObject OtTexturedMaterialClass::setScale(float s) {
	scale = s;
	return OtObject(this);
}


//
//	OtTexturedMaterialClass::setTransparent
//

OtObject OtTexturedMaterialClass::setTransparent(bool t) {
	transparent = t;
	return OtObject(this);
}


//
//	OtTexturedMaterialClass::setAmbient
//

OtObject OtTexturedMaterialClass::setAmbient(const std::string& c) {
	ambient = OtColorParseToVec3(c);
	return OtObject(this);
}


//
//	OtTexturedMaterialClass::setDiffuse
//

OtObject OtTexturedMaterialClass::setDiffuse(const std::string& c) {
	diffuse = OtColorParseToVec3(c);
	return OtObject(this);
}


//
//	OtTexturedMaterialClass::setSpecular
//

OtObject OtTexturedMaterialClass::setSpecular(const std::string& c) {
	specular = OtColorParseToVec3(c);
	return OtObject(this);
}


//
//	OtTexturedMaterialClass::setShininess
//

OtObject OtTexturedMaterialClass::setShininess(float s) {
	shininess = s;
	return OtObject(this);
}


//
//	OtTexturedMaterialClass::submit
//

void OtTexturedMaterialClass::submit(OtRenderer& renderer, bool wireframe, bool instancing) {
	if (renderer.inShadowmapPass()) {
		if (transparent) {
			scaleUniform.set(0, glm::vec4(scale, 0.0, 0.0, 0.0));
			scaleUniform.submit();
			textureSampler.submit(1, texture);

			if (instancing) {
				renderer.runShader(transparentShadowInstancingShader);

			} else {
				renderer.runShader(transparentShadowShader);
			}

		} else {
			if (instancing) {
				renderer.runShader(shadowInstancingShader);

			} else {
				renderer.runShader(shadowShader);
			}
		}

	} else {
		// submit uniform
		materialUniform.set(0, glm::vec4(ambient, scale));
		materialUniform.set(1, glm::vec4(diffuse, normalmap.isValid()));
		materialUniform.set(2, glm::vec4(specular, shininess));
		materialUniform.submit();

		// set samplers
		textureSampler.submit(1, texture);
		normalmapSampler.submit(2, normalmap);

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
//	OtTexturedMaterialClass::getMeta
//

OtType OtTexturedMaterialClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtTexturedMaterialClass>("TexturedMaterial", OtMaterialClass::getMeta());
		type->set("__init__", OtFunction::create(&OtTexturedMaterialClass::init));

		type->set("setTexture", OtFunction::create(&OtTexturedMaterialClass::setTexture));
		type->set("setNormalMap", OtFunction::create(&OtTexturedMaterialClass::setNormalMap));
		type->set("setScale", OtFunction::create(&OtTexturedMaterialClass::setScale));
		type->set("setTransparent", OtFunction::create(&OtTexturedMaterialClass::setTransparent));

		type->set("setAmbient", OtFunction::create(&OtTexturedMaterialClass::setAmbient));
		type->set("setDiffuse", OtFunction::create(&OtTexturedMaterialClass::setDiffuse));
		type->set("setSpecular", OtFunction::create(&OtTexturedMaterialClass::setSpecular));
		type->set("setShininess", OtFunction::create(&OtTexturedMaterialClass::setShininess));
	}

	return type;
}
