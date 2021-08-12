//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "wireframe.h"

#include "vertex_shader.h"
#include "fragment_shader.h"


//
//	OtWireframeClass::OtWireframeClass
//

OtWireframeClass::OtWireframeClass() {
	// initialize shader
	program = bgfx::createProgram(
		bgfx::createShader(bgfx::makeRef(vertex_shader_mtl, sizeof(vertex_shader_mtl))),
		bgfx::createShader(bgfx::makeRef(fragment_shader_mtl, sizeof(fragment_shader_mtl))),
		true);
}


//
//	OtWireframeClass::~OtWireframeClass
//

OtWireframeClass::~OtWireframeClass() {
	// release resources
	if (program.idx != bgfx::kInvalidHandle) {
		bgfx::destroy(program);
	}

	if (vertexBuffer.idx != bgfx::kInvalidHandle) {
		bgfx::destroy(vertexBuffer);
	}

	if (indexBuffer.idx != bgfx::kInvalidHandle) {
		bgfx::destroy(indexBuffer);
	}
}


//
//	OtWireframeClass::init
//

void OtWireframeClass::init(OtObject geom, OtObject mat) {
	setGeometry(geom);
	setMaterial(mat);
}


//
//	OtWireframeClass::setGeometry
//

void OtWireframeClass::setGeometry(OtObject object) {
	// ensure object is a geometry
	if (object->isKindOf("Geometry")) {
		geometry = object->cast<OtGeometryClass>();

	} else {
		OtExcept("Expected a [Geometry] object, not a [%s]", object->getType()->getName().c_str());
	}

	// release resources if required
	if (vertexBuffer.idx != bgfx::kInvalidHandle) {
		bgfx::destroy(vertexBuffer);
	}

	if (indexBuffer.idx != bgfx::kInvalidHandle) {
		bgfx::destroy(indexBuffer);
	}

	// create BGFX buffers
	vertexBuffer = geometry->getVertexBuffer();
	indexBuffer = geometry->getLineIndexBuffer();
}


//
//	OtWireframeClass::setMaterial
//

void OtWireframeClass::setMaterial(OtObject object) {
	// ensure object is a material
	if (object->isKindOf("Material")) {
		material = object->cast<OtMaterialClass>();

	} else {
		OtExcept("Expected a [Material] object, not a [%s]", object->getType()->getName().c_str());
	}
}


//
//	OtWireframeClass::render
//

void OtWireframeClass::render(int view, glm::mat4 parentTransform) {
	// sanity check
	if (!geometry || !material) {
		OtExcept("[Geometry] and/or [material] missing for [wireframe]");
	}

	// render back side

	// let parent class do its thing
	OtObject3dClass::render(view, parentTransform);

	// submit vertices and triangles
	bgfx::setVertexBuffer(0, vertexBuffer);
	bgfx::setIndexBuffer(indexBuffer);

	// setup material
	material->submit(view);

	bgfx::setState(
		BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_CULL_CCW | BGFX_STATE_MSAA |
		BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_PT_LINES |
		BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));

	// run shader
	bgfx::submit(view, program, bgfx::ViewMode::Default);

	// render front side

	// let parent class do its thing
	OtObject3dClass::render(view, parentTransform);

	// submit vertices and triangles
	bgfx::setVertexBuffer(0, vertexBuffer);
	bgfx::setIndexBuffer(indexBuffer);

	// setup material
	material->submit(view);

	bgfx::setState(
		BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_CULL_CW | BGFX_STATE_MSAA |
		BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_PT_LINES |
		BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));

	// run shader
	bgfx::submit(view, program, bgfx::ViewMode::Default);
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
