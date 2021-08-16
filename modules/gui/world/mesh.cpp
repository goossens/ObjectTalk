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

#include "mesh.h"
#include "mesh_shader.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(vs_mesh),
	BGFX_EMBEDDED_SHADER(fs_mesh),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtMeshClass::OtMeshClass
//

OtMeshClass::OtMeshClass() {
	// initialize shader
	bgfx::RendererType::Enum type = bgfx::getRendererType();

	program = bgfx::createProgram(
		bgfx::createEmbeddedShader(embeddedShaders, type, "vs_mesh"),
		bgfx::createEmbeddedShader(embeddedShaders, type, "fs_mesh"),
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

void OtMeshClass::render(int view, glm::mat4 parentTransform, int flag) {
	// let parent class do its thing
	OtObject3dClass::render(view, parentTransform);

	// submit vertices and triangles
	bgfx::setVertexBuffer(0, vertexBuffer);
	bgfx::setIndexBuffer(indexBuffer);

	// setup material
	material->submit(view);

	bgfx::setState(
		BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA |
		BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS |
		flag |
		BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));

	// run shader
	bgfx::submit(view, program, bgfx::ViewMode::Default);
}


//
//	OtMeshClass::render
//

void OtMeshClass::render(int view, glm::mat4 parentTransform) {
	// see if culling is desired
	if (!geometry->wantsCulling() || holes) {
		// render back side
		render(view, parentTransform, BGFX_STATE_CULL_CCW);
	}

	// render front side
	render(view, parentTransform, BGFX_STATE_CULL_CW);
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
