//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bgfx/embedded_shader.h"

#include "OtFunction.h"

#include "OtBackground.h"
#include "OtBackgroundShader.h"
#include "OtColor.h"
#include "OtFramework.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(OtBackgroundVS),
	BGFX_EMBEDDED_SHADER(OtBackgroundFS),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtBackgroundClass::OtBackgroundClass
//

OtBackgroundClass::OtBackgroundClass() {
	// create geometry
	plane = OtPlaneGeometryClass::create();
	plane->setWidth(2.0);
	plane->setHeight(2.0);

	// register uniform
	transformUniform = bgfx::createUniform("u_background_transform", bgfx::UniformType::Mat4);
	backgroundUniform = bgfx::createUniform("u_background", bgfx::UniformType::Vec4, 2);
	textureUniform = bgfx::createUniform("s_texture", bgfx::UniformType::Sampler);

	// initialize shader
	bgfx::RendererType::Enum type = bgfx::getRendererType();

	shader = bgfx::createProgram(
		bgfx::createEmbeddedShader(embeddedShaders, type, "OtBackgroundVS"),
		bgfx::createEmbeddedShader(embeddedShaders, type, "OtBackgroundFS"),
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
	object->expectKindOf("Texture");
	texture = object->cast<OtTextureClass>();
	return shared();
}


//
//	OtBackgroundClass::render
//

void OtBackgroundClass::render(OtRenderingContext* context) {
	// submit uniforms
	glm::mat4 transform = glm::mat4(1.0);
	OtFramework framework = OtFrameworkClass::instance();
	float sw = framework->getWidth();
	float sh = framework->getHeight();
	float scale = sh / sw;
	// transform = glm::scale(transform, glm::vec3(1.0, scale, 1.0));
	bgfx::setUniform(transformUniform, &transform);

	glm::vec4 uniforms[2];
	uniforms[0].x = texture ? 1.0 : 0.0;
	uniforms[1] = glm::vec4(color, 1.0);
	bgfx::setUniform(backgroundUniform, &uniforms, 2);

	(texture ? texture : OtTextureClass::dummy())->submit(0, textureUniform);

	// submit vertices and triangles
	bgfx::setVertexBuffer(0, plane->getVertexBuffer());
	bgfx::setIndexBuffer(plane->getTriangleIndexBuffer());

	// run shader
	bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA);
	bgfx::submit(context->view, shader);
}


//
//	OtBackgroundClass::getMeta
//

OtType OtBackgroundClass::getMeta() {
	static OtType type;

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
