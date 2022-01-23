//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>

#include "ot/function.h"

#include "mesh.h"
#include "matrix.h"

#include "fixedshader.h"
#include "coloredshader.h"
#include "texturedshader.h"
#include "blendmappedshader.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(vs_fixed),
	BGFX_EMBEDDED_SHADER(vsi_fixed),
	BGFX_EMBEDDED_SHADER(fs_fixed),
	BGFX_EMBEDDED_SHADER(vs_colored),
	BGFX_EMBEDDED_SHADER(vsi_colored),
	BGFX_EMBEDDED_SHADER(fs_colored),
	BGFX_EMBEDDED_SHADER(vs_textured),
	BGFX_EMBEDDED_SHADER(vsi_textured),
	BGFX_EMBEDDED_SHADER(fs_textured),
	BGFX_EMBEDDED_SHADER(vs_blendmapped),
	BGFX_EMBEDDED_SHADER(vsi_blendmapped),
	BGFX_EMBEDDED_SHADER(fs_blendmapped),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtMeshClass::~OtMeshClass
//

OtMeshClass::~OtMeshClass() {
	if (bgfx::isValid(shader)) {
		bgfx::destroy(shader);
	}
}


//
//	OtMeshClass::init
//

OtObject OtMeshClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 2:
				setMaterial(parameters[1]);

			case 1:
				setGeometry(parameters[0]);
				break;

			default:
				OtExcept("Too many parameters [%ld] for [Mesh] constructor (max 2)", count);
		}
	}

	return nullptr;
}


//
//	OtMeshClass::setGeometry
//

OtObject OtMeshClass::setGeometry(OtObject object) {
	object->expectKindOf("Geometry");
	geometry = object->cast<OtGeometryClass>();
	return shared();
}


//
//	OtMeshClass::setMaterial
//

OtObject OtMeshClass::setMaterial(OtObject object) {
	object->expectKindOf("Material");
	material = object->cast<OtMaterialClass>();
	return shared();
}


//
//	OtMeshClass::setWireframe
//

OtObject OtMeshClass::setWireframe(bool w) {
	wireframe = w;
	return shared();
}


//
//	OtMeshClass::setHoles
//

OtObject OtMeshClass::setHoles(bool h) {
	holes = h;
	return shared();
}


//
//	OtMeshClass::addInstance
//

OtObject OtMeshClass::addInstance(OtObject object) {
	object->expectKindOf("Matrix");
	instances.push_back(object->cast<OtMatrixClass>()->getComposite());
	return shared();
}


//
//	OtMeshClass::render
//

void OtMeshClass::render(OtRenderingContext* context, long flag) {
	// let parent class do its thing
	OtObject3dClass::render(context);

	// setup context
	context->submit();

	// setup material
	material->submit();

	// submit vertices and triangles/lines
	bgfx::setVertexBuffer(0, geometry->getVertexBuffer());

	if (wireframe) {
		bgfx::setIndexBuffer(geometry->getLineIndexBuffer());
		flag |= BGFX_STATE_PT_LINES;

	} else {
		bgfx::setIndexBuffer(geometry->getTriangleIndexBuffer());
	}

	// handle instancing (if required)
	if (instances.size()) {
		bgfx::InstanceDataBuffer idb;
		bgfx::allocInstanceDataBuffer(&idb, instances.size(), sizeof(glm::mat4));
		std::memcpy(idb.data, instances.data(), idb.size);
		bgfx::setInstanceDataBuffer(&idb);
	}

	// set rendering options
	bgfx::setState(
		BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA |
		BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS |
		flag |
		BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));

	// run shader
	bgfx::submit(context->view, shader);
}


//
//	OtMeshClass::render
//

void OtMeshClass::render(OtRenderingContext* context) {
	// sanity check
	if (!geometry || !material) {
		OtExcept("[Geometry] and/or [material] properties missing for [Mesh]");
	}

	// ensure we have the right shader
	auto mt = material->getType();
	auto instancing = instances.size() ? 10 : 0;

	if (materialType != mt + instancing) {
		materialType = mt + instancing;

		if (bgfx::isValid(shader)) {
			bgfx::destroy(shader);
		}

		// initialize shader
		bgfx::RendererType::Enum type = bgfx::getRendererType();

		if (mt == OtMaterialClass::BLENDMAPPED) {
			if (instancing) {
				shader = bgfx::createProgram(
					bgfx::createEmbeddedShader(embeddedShaders, type, "vsi_blendmapped"),
					bgfx::createEmbeddedShader(embeddedShaders, type, "fs_blendmapped"),
					true);

			} else {
				shader = bgfx::createProgram(
					bgfx::createEmbeddedShader(embeddedShaders, type, "vs_blendmapped"),
					bgfx::createEmbeddedShader(embeddedShaders, type, "fs_blendmapped"),
					true);
			}

		} else if (mt == OtMaterialClass::TEXTURED) {
			if (instancing) {
				shader = bgfx::createProgram(
					bgfx::createEmbeddedShader(embeddedShaders, type, "vsi_textured"),
					bgfx::createEmbeddedShader(embeddedShaders, type, "fs_textured"),
					true);

			} else {
				shader = bgfx::createProgram(
					bgfx::createEmbeddedShader(embeddedShaders, type, "vs_textured"),
					bgfx::createEmbeddedShader(embeddedShaders, type, "fs_textured"),
					true);
			}

		} else if (mt == OtMaterialClass::COLORED) {
			if (instancing) {
				shader = bgfx::createProgram(
					bgfx::createEmbeddedShader(embeddedShaders, type, "vsi_colored"),
					bgfx::createEmbeddedShader(embeddedShaders, type, "fs_colored"),
					true);

			} else {
				shader = bgfx::createProgram(
					bgfx::createEmbeddedShader(embeddedShaders, type, "vs_colored"),
					bgfx::createEmbeddedShader(embeddedShaders, type, "fs_colored"),
					true);
			}

		} else {
			if (instancing) {
				shader = bgfx::createProgram(
					bgfx::createEmbeddedShader(embeddedShaders, type, "vsi_fixed"),
					bgfx::createEmbeddedShader(embeddedShaders, type, "fs_fixed"),
					true);

			} else {
				shader = bgfx::createProgram(
					bgfx::createEmbeddedShader(embeddedShaders, type, "vs_fixed"),
					bgfx::createEmbeddedShader(embeddedShaders, type, "fs_fixed"),
					true);
			}
		}
	}

	// see if culling is desired
	if (wireframe || holes || !geometry->wantsCulling()) {
		// render back side
		render(context, BGFX_STATE_CULL_CCW);
	}

	// render front side
	render(context, BGFX_STATE_CULL_CW);
}


//
//	OtMeshClass::getMeta
//

OtType OtMeshClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtMeshClass>("Mesh", OtObject3dClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtMeshClass::init));

		type->set("setGeometry", OtFunctionClass::create(&OtMeshClass::setGeometry));
		type->set("setMaterial", OtFunctionClass::create(&OtMeshClass::setMaterial));
		type->set("setWireframe", OtFunctionClass::create(&OtMeshClass::setWireframe));
		type->set("setHoles", OtFunctionClass::create(&OtMeshClass::setHoles));
		type->set("addInstance", OtFunctionClass::create(&OtMeshClass::addInstance));
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
