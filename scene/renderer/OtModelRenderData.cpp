//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtModelRenderData.h"


//
//	OtModelRenderData::analyzeEntity
//

void OtModelRenderData::analyzeEntity(OtScene* scene, OtEntity ett) {
	// get reference to model
	entity = ett;
	component = &scene->getComponent<OtModelComponent>(entity);
	model = &component->asset->getModel();

	// determine current AABB in world space
	globalTransform = scene->getGlobalTransform(entity);
	worldAabb = model->getAABB().transform(globalTransform);
}


//
//	OtModelRenderData::analyzeCamera
//

void OtModelRenderData::analyzeCamera(size_t type, OtCamera& camera) {
	visible[type] = camera.isVisibleAABB(worldAabb);
}
