//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtArray.h"
#include "OtException.h"
#include "OtFunction.h"

#include "OtColor.h"
#include "OtColoredMaterial.h"


//
//	Named materials
//

struct OtMaterialProperties {
	const char* name;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

static OtMaterialProperties materials[] = {
	{ "emerald", glm::vec3(0.0215, 0.1745, 0.0215), glm::vec3(0.07568, 0.61424, 0.07568), glm::vec3(0.633, 0.727811, 0.633), 60.0 },
	{ "jade", glm::vec3(0.135, 0.2225, 0.1575), glm::vec3(0.54, 0.89, 0.63), glm::vec3(0.316228, 0.316228, 0.316228), 10.0 },
	{ "obsidian", glm::vec3(0.05375, 0.05, 0.06625), glm::vec3(0.18275, 0.17, 0.22525), glm::vec3(0.332741, 0.328634, 0.346435), 30.0 },
	{ "pearl", glm::vec3(0.25, 0.20725, 0.20725), glm::vec3(1, 0.829, 0.829), glm::vec3(0.296648, 0.296648, 0.296648), 8.8 },
	{ "ruby", glm::vec3(0.1745, 0.01175, 0.01175), glm::vec3(0.61424, 0.04136, 0.04136), glm::vec3(0.727811, 0.626959, 0.626959), 60.0 },
	{ "turquoise", glm::vec3(0.1, 0.18725, 0.1745), glm::vec3(0.396, 0.74151, 0.69102), glm::vec3(0.297254, 0.30829, 0.306678), 10.0 },
	{ "brass", glm::vec3(0.329412, 0.223529, 0.027451), glm::vec3(0.780392, 0.568627, 0.113725), glm::vec3(0.992157, 0.941176, 0.807843), 21.7 },
	{ "bronze", glm::vec3(0.2125, 0.1275, 0.054), glm::vec3(0.714, 0.4284, 0.18144), glm::vec3(0.393548, 0.271906, 0.166721), 20.0 },
	{ "chrome", glm::vec3(0.25, 0.25, 0.25), glm::vec3(0.4, 0.4, 0.4), glm::vec3(0.774597, 0.774597, 0.774597), 60.0 },
	{ "copper", glm::vec3(0.19125, 0.0735, 0.0225), glm::vec3(0.7038, 0.27048, 0.0828), glm::vec3(0.256777, 0.137622, 0.086014), 10.0 },
	{ "gold", glm::vec3(0.24725, 0.1995, 0.0745), glm::vec3(0.75164, 0.60648, 0.22648), glm::vec3(0.628281, 0.555802, 0.366065), 40.0 },
	{ "silver", glm::vec3(0.19225, 0.19225, 0.19225), glm::vec3(0.50754, 0.50754, 0.50754), glm::vec3(0.508273, 0.508273, 0.508273), 40.0 },
	{ "black plastic", glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.01, 0.01, 0.01), glm::vec3(0.50, 0.50, 0.50), 25.0 },
	{ "cyan plastic", glm::vec3(0.0, 0.1, 0.06), glm::vec3(0.0, 0.50980392, 0.50980392), glm::vec3(0.50196078, 0.50196078, 0.50196078), 25.0 },
	{ "green plastic", glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.1, 0.35, 0.1), glm::vec3(0.45, 0.55, 0.45), 25.0 },
	{ "red plastic", glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.5, 0.0, 0.0), glm::vec3(0.7, 0.6, 0.6), 25.0 },
	{ "white plastic", glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.55, 0.55, 0.55), glm::vec3(0.70, 0.70, 0.70), 25.0 },
	{ "yellow plastic", glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.5, 0.5, 0.0), glm::vec3(0.60, 0.60, 0.50), 25.0 },
	{ "black rubber", glm::vec3(0.02, 0.02, 0.02), glm::vec3(0.01, 0.01, 0.01), glm::vec3(0.4, 0.4, 0.4), 7.8 },
	{ "cyan rubber", glm::vec3(0.0, 0.05, 0.05), glm::vec3(0.4, 0.5, 0.5), glm::vec3(0.04, 0.7, 0.7), 7.8 },
	{ "green rubber", glm::vec3(0.0, 0.05, 0.0), glm::vec3(0.4, 0.5, 0.4), glm::vec3(0.04, 0.7, 0.04), 7.8 },
	{ "red rubber", glm::vec3(0.05, 0.0, 0.0), glm::vec3(0.5, 0.4, 0.4), glm::vec3(0.7, 0.04, 0.04), 7.8 },
	{ "white rubber", glm::vec3(0.05, 0.05, 0.05), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.7, 0.7, 0.7), 7.8 },
	{ "yellow rubber", glm::vec3(0.05, 0.05, 0.0), glm::vec3(0.5, 0.5, 0.4), glm::vec3(0.7, 0.7, 0.04), 7.8 }
};


//
//	OtColoredMaterialClass::init
//

void OtColoredMaterialClass::init(size_t count, OtObject* parameters) {
	if (count == 1) {
		setColor(parameters[0]->operator std::string());

	} else if (count == 3) {
		setColorRGB(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float());

	} else if (count == 4) {
		setColorRGB(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float());

		setOpacity(parameters[3]->operator float());

	} else {
		OtExcept("[ColoredMaterial] constructor expects 1, 3 or 4 arguments (not %ld)", count);
	}
}


//
//	OtColoredMaterialClass::setColor
//

OtObject OtColoredMaterialClass::setColor(const std::string& name) {
	// try to find named material
	for (const auto& material : materials) {
		if (name == material.name) {
			ambient = material.ambient;
			diffuse = material.diffuse;
			specular = material.specular;
			shininess = material.shininess;
			return shared();
		}
	}
	// parse CSS style color
	color = OtColorParseToVec3(name);
	return shared();
}


//
//	OtColoredMaterialClass::setColorRGB
//

OtObject OtColoredMaterialClass::setColorRGB(float r, float g, float b) {
	color = glm::vec3(r, g, b);
	return shared();
}


//
//	OtColoredMaterialClass::setOpacity
//

OtObject OtColoredMaterialClass::setOpacity(float o) {
	opacity = o;
	return shared();
}


//
//	OtMaterialClass::setAmbient
//

OtObject OtColoredMaterialClass::setAmbient(const std::string& c) {
	ambient = OtColorParseToVec3(c);
	return shared();
}


//
//	OtColoredMaterialClass::setDiffuse
//

OtObject OtColoredMaterialClass::setDiffuse(const std::string& c) {
	diffuse = OtColorParseToVec3(c);
	return shared();
}


//
//	OtColoredMaterialClass::setSpecular
//

OtObject OtColoredMaterialClass::setSpecular(const std::string& c) {
	specular = OtColorParseToVec3(c);
	return shared();
}


//
//	OtColoredMaterialClass::setShininess
//

OtObject OtColoredMaterialClass::setShininess(float s) {
	shininess = s;
	return shared();
}


//
//	OtColoredMaterialClass::submit
//

void OtColoredMaterialClass::submit(OtRenderer& renderer, bool wireframe, bool instancing) {
	if (renderer.inShadowmapPass()) {
		if (instancing) {
			renderer.runShader(shadowInstancingShader);

		} else {
			renderer.runShader(shadowShader);
		}

	} else {
		// submit uniform
		uniform.set(0, glm::vec4(color, opacity));
		uniform.set(1, glm::vec4(ambient, 0.0));
		uniform.set(2, glm::vec4(diffuse, 0.0));
		uniform.set(3, glm::vec4(specular, shininess));
		uniform.submit();

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
//	OtColoredMaterialClass::getMeta
//

OtType OtColoredMaterialClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtColoredMaterialClass>("ColoredMaterial", OtMaterialClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtColoredMaterialClass::init));

		type->set("setColor", OtFunctionClass::create(&OtColoredMaterialClass::setColor));
		type->set("setColorRGB", OtFunctionClass::create(&OtColoredMaterialClass::setColorRGB));
		type->set("setOpacity", OtFunctionClass::create(&OtColoredMaterialClass::setOpacity));

		type->set("setAmbient", OtFunctionClass::create(&OtColoredMaterialClass::setAmbient));
		type->set("setDiffuse", OtFunctionClass::create(&OtColoredMaterialClass::setDiffuse));
		type->set("setSpecular", OtFunctionClass::create(&OtColoredMaterialClass::setSpecular));
		type->set("setShininess", OtFunctionClass::create(&OtColoredMaterialClass::setShininess));
	}

	return type;
}


//
//	OtColoredMaterialClass::create
//

OtColoredMaterial OtColoredMaterialClass::create() {
	OtColoredMaterial material = std::make_shared<OtColoredMaterialClass>();
	material->setType(getMeta());
	return material;
}


//
//	OtColoredMaterialClass::getDefaultMaterialNames
//

OtObject OtColoredMaterialClass::getDefaultMaterialNames() {
	OtArray array = OtArrayClass::create();

	for (const auto& material : materials) {
		array->append(OtStringClass::create(material.name));
	}

	return array;
}
