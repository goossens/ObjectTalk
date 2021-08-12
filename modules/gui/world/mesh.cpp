//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "mesh.h"

#include "vertex_shader.h"
#include "fragment_shader.h"


//
//	OtMeshClass::OtMeshClass
//

OtMeshClass::OtMeshClass() {
	// initialize shader
	program = bgfx::createProgram(
		bgfx::createShader(bgfx::makeRef(vertex_shader_mtl, sizeof(vertex_shader_mtl))),
		bgfx::createShader(bgfx::makeRef(fragment_shader_mtl, sizeof(fragment_shader_mtl))),
		true);
}


//
//	OtMeshClass::~OtMeshClass
//

OtMeshClass::~OtMeshClass() {
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
//	OtMeshClass::init
//

OtObject OtMeshClass::init(size_t count, OtObject* parameters) {
	// set attributes
	switch (count) {
		case 3:
			holes = parameters[2]->operator bool();

		case 2:
			setMaterial(parameters[1]);
			setGeometry(parameters[0]);
			break;

		case 1:
			OtExcept("[Geometry] or [material] missing in [mesh] contructor");

		case 0:
			OtExcept("[Geometry] and [material] missing in [mesh] contructor");

		default:
			OtExcept("Too many parameters [%ld] for [Mesh] contructor (max 3)", count);
	}

	return nullptr;
}


//
//	OtMeshClass::setGeometry
//

void OtMeshClass::setGeometry(OtObject object) {
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
	indexBuffer = geometry->getTriangleIndexBuffer();
}


//
//	OtMeshClass::setMaterial
//

void OtMeshClass::setMaterial(OtObject object) {
	// ensure object is a material
	if (object->isKindOf("Material")) {
		material = object->cast<OtMaterialClass>();

	} else {
		OtExcept("Expected a [Material] object, not a [%s]", object->getType()->getName().c_str());
	}
}


//
//	OtMeshClass::render
//

void OtMeshClass::render(int view, glm::mat4 parentTransform) {
	// see if culling is desired
	if (!geometry->wantsCulling() || holes) {
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
			BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS |
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
			BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS |
			BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));

		// run shader
		bgfx::submit(view, program, bgfx::ViewMode::Default);

	} else {
		// let parent class do its thing
		OtObject3dClass::render(view, parentTransform);

		// submit vertices and triangles
		bgfx::setVertexBuffer(0, vertexBuffer);
		bgfx::setIndexBuffer(indexBuffer);

		// setup material
		material->submit(view);

		// run shader
		bgfx::submit(view, program, bgfx::ViewMode::Default);
	}
}


//
//	OtMeshClass::getMeta
//

OtType OtMeshClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtMeshClass>("Mesh", OtObject3dClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtMeshClass::init));
		type->set("setGeometry", OtFunctionClass::create(&OtMeshClass::setGeometry));
		type->set("setMaterial", OtFunctionClass::create(&OtMeshClass::setMaterial));
	}

	return type;
}


//
//	OtMeshClass::create
//

OtMesh OtMeshClass::create() {
	OtMesh mesh = std::make_shared<OtMeshClass>();
	mesh->setType(getMeta());
	return mesh;
}
