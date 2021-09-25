//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bgfx/embedded_shader.h"

#include "ot/function.h"

#include "background.h"
#include "backgroundshader.h"
#include "application.h"
#include "color.h"
#include "plane.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(vs_background),
	BGFX_EMBEDDED_SHADER(fs_background),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtBackgroundClass::OtBackgroundClass
//

OtBackgroundClass::OtBackgroundClass() {
	// create geometry
	plane = OtPlaneClass::create();
	plane->setWidth(2.0);
	plane->setHeight(2.0);

	// register uniform
	transformUniform = bgfx::createUniform("u_background_transform", bgfx::UniformType::Mat4);
	backgroundUniform = bgfx::createUniform("u_background", bgfx::UniformType::Vec4, 2);
	textureUniform = bgfx::createUniform("s_texture", bgfx::UniformType::Sampler);

	// create a dummy texture
	dummy = bgfx::createTexture2D(
		1, 1, false, 1,
		bgfx::TextureFormat::RGBA32F,
		BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
		nullptr);

	// initialize shader
	bgfx::RendererType::Enum type = bgfx::getRendererType();

	shader = bgfx::createProgram(
		bgfx::createEmbeddedShader(embeddedShaders, type, "vs_background"),
		bgfx::createEmbeddedShader(embeddedShaders, type, "fs_background"),
		true);
}


//
//	OtBackgroundClass::~OtBackgroundClass
//

OtBackgroundClass::~OtBackgroundClass() {
	// release resources
	bgfx::destroy(transformUniform);
	bgfx::destroy(backgroundUniform);
	bgfx::destroy(textureUniform);
	bgfx::destroy(shader);
}


//
//	OtBackgroundClass::init
//

OtObject OtBackgroundClass::init(size_t count, OtObject* parameters) {
	if (count == 1) {
		setTexture(parameters[0]);

	} else if (count != 0) {
		OtExcept("[Background] constructor expects 0 or 1 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtBackgroundClass::setColor
//

OtObject OtBackgroundClass::setColor(const std::string& name) {
	color = OtColorParseToVec3(name);
	return shared();
}


//
//	OtBackgroundClass::setTexture
//

OtObject OtBackgroundClass::setTexture(OtObject object) {
	// ensure object is a material
	if (object->isKindOf("Texture")) {
		texture = object->cast<OtTextureClass>();

	} else {
		OtExcept("Expected a [Texture] object, not a [%s]", object->getType()->getName().c_str());
	}

	return shared();
}


//
//	OtBackgroundClass::render
//

void OtBackgroundClass::render(int view, OtCamera camera, glm::mat4 parentTransform) {
	// submit uniforms
	glm::mat4 transform = glm::mat4(1.0);
	float scale = (float) OtApplicationClass::getHeight() / (float) OtApplicationClass::getWidth();
	// transform = glm::scale(transform, glm::vec3(1.0, scale, 1.0));
	bgfx::setUniform(transformUniform, &transform);

	glm::vec4 uniforms[2];
	uniforms[0].x = texture ? 1.0 : 0.0;
	uniforms[1] = glm::vec4(color, 1.0);
	bgfx::setUniform(backgroundUniform, &uniforms, 2);

	if (texture) {
		texture->submit(0, textureUniform);

	} else {
		bgfx::setTexture(0, textureUniform, dummy);
	}

	// submit vertices and triangles
	bgfx::setVertexBuffer(0, plane->getVertexBuffer());
	bgfx::setIndexBuffer(plane->getTriangleIndexBuffer());

	// run shader
	bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA);
	bgfx::submit(view, shader);
}


//
//	OtBackgroundClass::getMeta
//

OtType OtBackgroundClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtBackgroundClass>("Background", OtSceneObjectClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtBackgroundClass::init));
		type->set("setColor", OtFunctionClass::create(&OtBackgroundClass::setColor));
		type->set("setTexture", OtFunctionClass::create(&OtBackgroundClass::setTexture));
	}

	return type;
}


//
//	OtBackgroundClass::create
//

OtBackground OtBackgroundClass::create() {
	OtBackground background = std::make_shared<OtBackgroundClass>();
	background->setType(getMeta());
	return background;
}
