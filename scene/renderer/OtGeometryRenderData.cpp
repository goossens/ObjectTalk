//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtGeometryRenderData.h"


//
//	OtGeometryRenderData::analyzeEntity
//

void OtGeometryRenderData::analyzeEntity(OtScene* scene, OtEntity ett) {
	// track changes since last frame
	changed = false;

	// get reference to geometry
	entity = ett;
	component = &scene->getComponent<OtGeometryComponent>(entity);
	geometry = &component->asset->getGeometry();

	// see if geometry changed
	if (geometry->getVersion() != geometryVersion) {
		geometryVersion = geometry->getVersion();
		changed = true;
	}

	// determine current AABB in world space and see if it's different from the last frame?
	globalTransform = scene->getGlobalTransform(entity);
	auto aabb = geometry->getAABB().transform(globalTransform);

	if (aabb != worldAabb) {
		worldAabb = aabb;
		changed = true;
	}

	// see if a material is specified (if not, we use a dummy)
	material = scene->hasComponent<OtMaterialComponent>(entity)
		? scene->getComponent<OtMaterialComponent>(entity).material.get()
		: &dummyMaterial;

	// see if geometry is instancing
	if (scene->hasComponent<OtInstancingComponent>(entity)) {
		auto& ic = scene->getComponent<OtInstancingComponent>(entity);

		if (ic.asset.isNull()) {
			instances = nullptr;

		} else {
			instances = &ic.asset->getInstances();
		}

	} else {
		instances = nullptr;
	}

	// see if geometry casts a shadow
	if (castShadow != component->castShadow) {
		castShadow = component->castShadow;
		changed = true;
	}
}


//
//	OtGeometryRenderData::analyzeCamera
//

void OtGeometryRenderData::analyzeCamera(size_t type, OtCamera& camera) {
	// find view
	auto& view = cameras[type];

	// geometries that don't cast a shadow are ignored for shadow cameras
	if (isShadowCamera(type) && !castShadow) {
		view.visible = false;

	} else {
		// track changes
		bool updated = changed;

		// see if camera/view frustum has changed
		if (view.frustum != camera.frustum) {
			view.frustum = camera.frustum;
			updated = true;
		}

		// is this a case of instancing?
		if (instances) {
			// see if instances have changed
			if (instances->getVersion() != view.instancesVersion) {
				view.instancesVersion = instances->getVersion();
				updated = true;
			}

			// update list of visible instances (if required)
			if (updated) {
				view.visible = instances->getVisible(camera, worldAabb, view.visibleInstances);
			}

		} else {
			// update visibility (if required)
			view.visibleInstances.clear();

			if (updated) {
				view.visible = camera.isVisibleAABB(worldAabb);
			}
		}
	}
}
