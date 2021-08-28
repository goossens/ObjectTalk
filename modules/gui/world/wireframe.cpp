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

#include "wireframe.h"
#include "meshshader.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(vs_mesh),
	BGFX_EMBEDDED_SHADER(fs_mesh),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtWireframeClass::OtWireframeClass
//

OtWireframeClass::OtWireframeClass() {
	// initialize shader
	bgfx::RendererType::Enum type = bgfx::getRendererType();

	program = bgfx::createProgram(
		bgfx::createEmbeddedShader(embeddedShaders, type, "vs_mesh"),
		bgfx::createEmbeddedShader(embeddedShaders, type, "fs_mesh"),
		true);
}


//
//	OtWireframeClass::~OtWireframeClass
//

OtWireframeClass::~OtWireframeClass() {
	// release resources
	bgfx::destroy(program);
}


//
//	OtWireframeClass::init
//

OtObject OtWireframeClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 2:
				setMaterial(parameters[1]);

			case 1:
				setGeometry(parameters[0]);
				break;

			default:
				OtExcept("Too many parameters [%ld] for [Wireframe] constructor (max 2)", count);
		}
	}

	return nullptr;
}


//
//	OtWireframeClass::setGeometry
//

OtObject OtWireframeClass::setGeometry(OtObject object) {
	// ensure object is a geometry
	if (object->isKindOf("Geometry")) {
		geometry = object->cast<OtGeometryClass>();

	} else {
		OtExcept("Expected a [Geometry] object, not a [%s]", object->getType()->getName().c_str());
	}

	return shared();
}


//
//	OtWireframeClass::setMaterial
//

OtObject OtWireframeClass::setMaterial(OtObject object) {
	// ensure object is a material
	if (object->isKindOf("Material")) {
		material = object->cast<OtMaterialClass>();

	} else {
		OtExcept("Expected a [Material] object, not a [%s]", object->getType()->getName().c_str());
	}

	return shared();
}


//
//	OtWireframeClass::render
//

void OtWireframeClass::render(int view, glm::mat4 parentTransform, long flag) {
	// let parent class do its thing
	OtObject3dClass::render(view, parentTransform);

	// submit vertices and triangles
	bgfx::setVertexBuffer(0, geometry->getVertexBuffer());
	bgfx::setIndexBuffer(geometry->getLineIndexBuffer());

	// setup material
	material->submit(view);

	bgfx::setState(
		BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA |
		BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_PT_LINES |
		flag |
		BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));

	// run shader
	bgfx::submit(view, program, bgfx::ViewMode::Default);
}


//
//	OtWireframeClass::render
//

void OtWireframeClass::render(int view, glm::mat4 parentTransform) {
	// sanity check
	if (!geometry || !material) {
		OtExcept("[Geometry] and/or [material] missing for [wireframe]");
	}

	// render both sides
	render(view, parentTransform, BGFX_STATE_CULL_CCW);
	render(view, parentTransform, BGFX_STATE_CULL_CW);
}


//
//	OtWireframeClass::getMeta
//

OtType OtWireframeClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtWireframeClass>("Wireframe", OtObject3dClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtWireframeClass::init));

		type->set("setGeometry", OtFunctionClass::create(&OtWireframeClass::setGeometry));
		type->set("setMaterial", OtFunctionClass::create(&OtWireframeClass::setMaterial));
	}

	return type;
}


//
//	OtWireframeClass::create
//

OtWireframe OtWireframeClass::create() {
	OtWireframe wireframe = std::make_shared<OtWireframeClass>();
	wireframe->setType(getMeta());
	return wireframe;
}
