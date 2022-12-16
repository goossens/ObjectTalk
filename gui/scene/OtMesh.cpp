//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>

#include "glm/ext.hpp"

#include "OtFunction.h"

#include "OtGlm.h"
#include "OtMesh.h"
#include "OtMatrix.h"


//
//	OtMeshClass::init
//

void OtMeshClass::init(size_t count, OtObject* parameters) {
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

void OtMeshClass::render(OtRenderer& renderer, long flag) {
	// determine visibility
	OtAABB aabb = geometry->getAABB();
	glm::mat4 transform = getGlobalTransform();
	aabb = aabb->transform(transform);

	// don't bother if we're out of sight
	if (renderer.getCamera()->isVisibleAABB(aabb)) {
		// set transformation
		renderer.setTransform(transform);

		// submit vertices and triangles/lines
		if (wireframe) {
			geometry->submitLines();
			flag |= BGFX_STATE_PT_LINES;

		} else {
			geometry->submitTriangles();
		}

		// handle instancing (if required)
		if (instances.size() > 0) {
			renderer.setInstanceData(instances.data(), instances.size(), sizeof(glm::mat4));
		}

		// set rendering options
		renderer.setState(
			BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA |
			BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS |
			flag |
			BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));

		// setup context and material (material will invoke shader)
		renderer.submit(receivesShadow());
		material->submit(renderer, instances.size() > 0);
	}
}


//
//	OtMeshClass::render
//

void OtMeshClass::render(OtRenderer& renderer) {
	// sanity check
	if (!geometry) {
		OtExcept("[Geometry] missing for [Mesh]");
	}

	if (!material) {
		OtExcept("[Material] missing for [Mesh]");
	}

	// don't render if this is a shadowmap and we cast no shadow
	if (!renderer.inShadowmapPass() || castShadowFlag) {
		// see if we need to render the back side?
		if (wireframe || material->isBackSided()) {
			render(renderer, BGFX_STATE_CULL_CCW);
		}

		// see if we need to render the front side?
		if (material->isFrontSided()) {
			render(renderer, BGFX_STATE_CULL_CW);
		}
	}
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
