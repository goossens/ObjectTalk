//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
	geometry = OtGeometry(object);
	return OtObject(this);
}


//
//	OtMeshClass::setMaterial
//

OtObject OtMeshClass::setMaterial(OtObject object) {
	object->expectKindOf("Material");
	material = OtMaterial(object);
	return OtObject(this);
}


//
//	OtMeshClass::setWireframe
//

OtObject OtMeshClass::setWireframe(bool w) {
	wireframe = w;
	return OtObject(this);
}


//
//	OtMeshClass::addInstance
//

OtObject OtMeshClass::addInstance(OtObject object) {
	object->expectKindOf("Matrix");
	instances.push_back(OtMatrix(object)->getComposite());
	return OtObject(this);
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
		// determine visibility
		OtAABB aabb = geometry->getAABB();
		glm::mat4 transform = getGlobalTransform();
		aabb = aabb.transform(transform);

		// don't bother if we're out of sight
		if (true) {
//		if (renderer.getCamera()->isVisibleAABB(aabb)) {
			// set transformation
			renderer.setTransform(transform);

			// handle instancing (if required)
			if (instances.size() > 0) {
				renderer.setInstanceData(instances.data(), instances.size(), sizeof(glm::mat4));
			}

			// submit triangles or lines
			if (wireframe) {
				geometry->submitLines();

			} else {
				geometry->submitTriangles();
			}

			// setup context and material (material will invoke shader)
			renderer.submit(receivesShadow());
			material->submit(renderer, wireframe, instances.size() > 0);
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
		type->set("__init__", OtFunction::create(&OtMeshClass::init));

		type->set("setGeometry", OtFunction::create(&OtMeshClass::setGeometry));
		type->set("setMaterial", OtFunction::create(&OtMeshClass::setMaterial));
		type->set("setWireframe", OtFunction::create(&OtMeshClass::setWireframe));
		type->set("addInstance", OtFunction::create(&OtMeshClass::addInstance));
	}

	return type;
}
