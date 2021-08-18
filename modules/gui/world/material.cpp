//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bx/file.h"

#include "ot/array.h"
#include "ot/exception.h"
#include "ot/function.h"

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
	materialUniform = bgfx::createUniform("u_material", bgfx::UniformType::Vec4, 4);
	transformUniform = bgfx::createUniform("u_uv_transform", bgfx::UniformType::Mat3);
	textureUniform = bgfx::createUniform("s_texture", bgfx::UniformType::Sampler);

	// create a dummy texture
	dummy = bgfx::createTexture2D(
		1, 1, false, 1,
		bgfx::TextureFormat::RGBA32F,
		BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
		nullptr);
}


//
//	OtViewClass::~OtViewClass
//

OtMaterialClass::~OtMaterialClass() {
	// release resources
	if (materialUniform.idx != bgfx::kInvalidHandle) {
		bgfx::destroy(materialUniform);
	}

	if (transformUniform.idx != bgfx::kInvalidHandle) {
		bgfx::destroy(transformUniform);
	}

	if (textureUniform.idx != bgfx::kInvalidHandle) {
		bgfx::destroy(textureUniform);
	}

	if (image) {
		bimg::imageFree(image);
	}

	if (texture.idx != bgfx::kInvalidHandle) {
		bgfx::destroy(texture);
	}

	if (dummy.idx != bgfx::kInvalidHandle) {
		bgfx::destroy(dummy);
	}
}


//
//	OtMaterialClass::init
//

void OtMaterialClass::init(const std::string& name) {
	if (name == "vertex") {
		vertex = true;

	} else if (name.rfind("texture:", 0) == 0) {
		// load named texture
		std::string file = name.substr(8);
		static bx::DefaultAllocator allocator;
		static bx::FileReader reader;

		if (!bx::open(&reader, file.c_str())) {
			OtExcept("Can't open texture [%s]", file.c_str());
		}

		uint32_t size = (uint32_t) bx::getSize(&reader);
		void* data = BX_ALLOC(&allocator, size);
		bx::read(&reader, data, size);
		bx::close(&reader);

		image = bimg::imageParse(&allocator, data, size);

		if (!image)  {
			OtExcept("Can't process texture in [%s]", file.c_str());
		}

		const bgfx::Memory* mem = bgfx::makeRef(image->m_data, image->m_size);
		BX_FREE(&allocator, data);

		if (bgfx::isTextureValid(0, false, image->m_numLayers, bgfx::TextureFormat::Enum(image->m_format), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE)) {
			texture = bgfx::createTexture2D(
				uint16_t(image->m_width),
				uint16_t(image->m_height),
				1 < image->m_numMips,
				image->m_numLayers,
				bgfx::TextureFormat::Enum(image->m_format),
				BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
				mem);

		} else {
			OtExcept("Invalid texture format in [%s]", file.c_str());
		}

	} else if (name.rfind("color:", 0) == 0) {
		// parse CSS style color
		glm::vec3 color = OtColorParseToVec3(name.substr(6));
		ambient = color * 0.4f;
		diffuse = color * 0.6f;
		specular = color * 0.4f;

	} else {
		// try to find named material
		for (const auto& material : materials) {
			if (name == material.name) {
				ambient = material.ambient;
				diffuse = material.diffuse;
				specular = material.specular;
				shininess = material.shininess;
				return;
			}
		}

		OtExcept("Unknown material [%s]", name.c_str());
	}
}


//
//	OtMaterialClass::setUvTransform
//

void OtMaterialClass::setUvTransform(double ox, double oy, double rx, double ry, double r, double cx, double cy) {
	// specifiy a new UV coordinate transformation matrix
	auto c = std::cos(r);
	auto s = std::sin(r);

	uvTransform = glm::mat3(
		rx * c, rx * s, -rx * (c * cx + s * cy) + cx + ox,
		-ry * s, ry * c, -ry * (-s * cx + c * cy) + cy + oy,
		0.0, 0.0, 1.0
	);
}

//
//	OtMaterialClass::submit
//

void OtMaterialClass::submit(int view) {
	// pass material information
	glm::vec4 uniforms[4];
	uniforms[0] = glm::vec4(ambient, transparency);
	uniforms[1] = glm::vec4(diffuse, transparency);
	uniforms[2] = glm::vec4(specular, transparency);
	uniforms[3].x = vertex;
	uniforms[3].y = image ? 1.0 : 0.0;
	uniforms[3].z = shininess;
	bgfx::setUniform(materialUniform, &uniforms, 4);

	// pass UV transformation
	bgfx::setUniform(transformUniform, &uvTransform);

	// pass texture
	if (texture.idx != bgfx::kInvalidHandle) {
		bgfx::setTexture(0, textureUniform, texture);

	} else {
		bgfx::setTexture(0, textureUniform, dummy);
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

		type->set("setAmbientRGB", OtFunctionClass::create(&OtMaterialClass::setAmbientRGB));
		type->set("setAmbientCSS", OtFunctionClass::create(&OtMaterialClass::setAmbientCSS));
		type->set("setDiffuseRGB", OtFunctionClass::create(&OtMaterialClass::setDiffuseRGB));
		type->set("setDiffuseCSS", OtFunctionClass::create(&OtMaterialClass::setDiffuseCSS));
		type->set("setSpecularRGB", OtFunctionClass::create(&OtMaterialClass::setSpecularRGB));
		type->set("setSpecularCSS", OtFunctionClass::create(&OtMaterialClass::setSpecularCSS));

		type->set("setShininess", OtFunctionClass::create(&OtMaterialClass::setShininess));
		type->set("setTransparency", OtFunctionClass::create(&OtMaterialClass::setTransparency));

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

OtMaterial OtMaterialClass::create(const tinyobj::material_t& mat) {
	OtMaterial material = create();

	material->setAmbientRGB(mat.ambient[0], mat.ambient[1], mat.ambient[2]);
	material->setDiffuseRGB(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
	material->setSpecularRGB(mat.specular[0], mat.specular[1], mat.specular[2]);
	material->shininess = mat.shininess;
	material->shininess = mat.dissolve;

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
