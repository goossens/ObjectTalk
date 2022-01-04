//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/array.h"
#include "ot/exception.h"
#include "ot/function.h"

#include "color.h"
#include "material.h"


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
//	OtMaterialClass::OtMaterialClass
//

OtMaterialClass::OtMaterialClass() {
	// register uniforms
	materialUniform = bgfx::createUniform("u_material", bgfx::UniformType::Vec4, 5);
	transformUniform = bgfx::createUniform("u_uv_transform", bgfx::UniformType::Mat3);
	textureUniform = bgfx::createUniform("s_texture", bgfx::UniformType::Sampler);
}


//
//	OtMaterialClass::~OtMaterialClass
//

OtMaterialClass::~OtMaterialClass() {
	// release resources
	bgfx::destroy(materialUniform);
	bgfx::destroy(transformUniform);
	bgfx::destroy(textureUniform);
}


//
//	OtMaterialClass::init
//

OtObject OtMaterialClass::init(size_t count, OtObject* parameters) {
	if (count == 2) {
		std::string type = parameters[0]->operator std::string();
		std::string material = parameters[1]->operator std::string();

		if (type == "fixed") {
			setFixed(material);

		} else if (type == "material") {
			setMaterial(material);

		} else if (type == "color") {
			setColor(material);

		} else if (type == "texture") {
			setTexture(parameters[1]);

		} else if (type == "blendmap") {
			setBlendMap(parameters[1]);

		} else {
			OtExcept("Invalid material type [%s]", type.c_str());
		}

	} else if (count != 0) {
		OtExcept("[Material] constructor expects 0 or 2 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtMaterialClass::setFixed
//

OtObject OtMaterialClass::setFixed(const std::string& name) {
	// parse CSS style color
	fixed = true;
	color = OtColorParseToVec3(name);
	return shared();
}


//
//	OtMaterialClass::setMaterial
//

OtObject OtMaterialClass::setMaterial(const std::string& name) {
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

	OtExcept("Unknown material [%s]", name.c_str());
	return shared();
}


//
//	OtMaterialClass::setColor
//

OtObject OtMaterialClass::setColor(const std::string& name) {
	// parse CSS style color
	color = OtColorParseToVec3(name);
	return shared();
}


//
//	OtMaterialClass::setAmbient
//

OtObject OtMaterialClass::setAmbient(const std::string c) {
	ambient = OtColorParseToVec3(c);
	return shared();
}


//
//	OtMaterialClass::setDiffuse
//

OtObject OtMaterialClass::setDiffuse(const std::string c) {
	diffuse = OtColorParseToVec3(c);
	return shared();
}


//
//	OtMaterialClass::setSpecular
//

OtObject OtMaterialClass::setSpecular(const std::string c) {
	specular = OtColorParseToVec3(c);
	return shared();
}


//
//	OtMaterialClass::setShininess
//

OtObject OtMaterialClass::setShininess(float s) {
	shininess = s;
	return shared();
}


//
//	OtMaterialClass::setTransparency
//

OtObject OtMaterialClass::setTransparency(float t) {
	transparency = t;
	return shared();
}


//
//	OtMaterialClass::setUvTransform
//

OtObject OtMaterialClass::setUvTransform(float ox, float oy, float rx, float ry, float r, float cx, float cy) {
	// specifiy a new UV coordinate transformation matrix
	auto c = std::cos(r);
	auto s = std::sin(r);

	uvTransform = glm::mat3(
		rx * c, rx * s, -rx * (c * cx + s * cy) + cx + ox,
		-ry * s, ry * c, -ry * (-s * cx + c * cy) + cy + oy,
		0.0, 0.0, 1.0
	);

	return shared();
}


//
//	OtMaterialClass::setTexture
//

OtObject OtMaterialClass::setTexture(OtObject object) {
	// ensure object is a texture
	if (!object->isKindOf("Texture")) {
		OtExcept("Expected a [Texture] object, not a [%s]", object->getType()->getName().c_str());
	}

	texture = object->cast<OtTextureClass>();
	return shared();
}


//
//	OtMaterialClass::setBlendMap
//

OtObject OtMaterialClass::setBlendMap(OtObject object) {
	// ensure object is a blendmap
	if (!object->isKindOf("BlendMap")) {
		OtExcept("Expected a [BlendMap] object, not a [%s]", object->getType()->getName().c_str());
	}

	blendmap = object->cast<OtBlendMapClass>();
	return shared();
}


//
//	OtMaterialClass::submit
//

void OtMaterialClass::submit() {
	if (blendmap) {
		blendmap->submit();
		bgfx::setUniform(transformUniform, &uvTransform);

	} else if (texture) {
		texture->submit(0, textureUniform);
		bgfx::setUniform(transformUniform, &uvTransform);
	}

	// pass material information
	glm::vec4 uniforms[5];
	uniforms[0] = glm::vec4(color, transparency);
	uniforms[1] = glm::vec4(ambient, transparency);
	uniforms[2] = glm::vec4(diffuse, transparency);
	uniforms[3] = glm::vec4(specular, transparency);
	uniforms[4].x = shininess;
	bgfx::setUniform(materialUniform, &uniforms, 5);
}


//
//	OtMaterialClass::getType
//

int OtMaterialClass::getType() {
	if (blendmap) {
		return BLENDMAPPED;

	} else if (texture) {
		return TEXTURED;

	} else if (fixed) {
		return FIXED;

	} else {
		return COLORED;
	}
}


//
//	OtMaterialClass::getMeta
//

OtType OtMaterialClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtMaterialClass>("Material", OtGuiClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtMaterialClass::init));

		type->set("setFixed", OtFunctionClass::create(&OtMaterialClass::setFixed));

		type->set("setMaterial", OtFunctionClass::create(&OtMaterialClass::setMaterial));
		type->set("setColor", OtFunctionClass::create(&OtMaterialClass::setColor));

		type->set("setAmbient", OtFunctionClass::create(&OtMaterialClass::setAmbient));
		type->set("setDiffuse", OtFunctionClass::create(&OtMaterialClass::setDiffuse));
		type->set("setSpecular", OtFunctionClass::create(&OtMaterialClass::setSpecular));

		type->set("setShininess", OtFunctionClass::create(&OtMaterialClass::setShininess));
		type->set("setTransparency", OtFunctionClass::create(&OtMaterialClass::setTransparency));

		type->set("setTexture", OtFunctionClass::create(&OtMaterialClass::setTexture));
		type->set("setBlendMap", OtFunctionClass::create(&OtMaterialClass::setBlendMap));
		type->set("setUvTransform", OtFunctionClass::create(&OtMaterialClass::setUvTransform));
	}

	return type;
}


//
//	OtMaterialClass::create
//

OtMaterial OtMaterialClass::create() {
	OtMaterial material = std::make_shared<OtMaterialClass>();
	material->setType(getMeta());
	return material;
}


//
//	OtMaterialClass::getDefaultMaterials
//

OtObject OtMaterialClass::getDefaultMaterials() {
	OtArray array = OtArrayClass::create();

	for (const auto& material : materials) {
		array->append(OtStringClass::create(material.name));
	}

	return array;
}
