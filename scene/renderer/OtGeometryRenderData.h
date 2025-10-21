//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <array>
#include <vector>

#include "glm/glm.hpp"

#include "OtAABB.h"
#include "OtCascadedShadowMap.h"
#include "OtGeometry.h"
#include "OtFrustum.h"
#include "OtInstances.h"
#include "OtMaterial.h"

#include "OtScene.h"

#include "OtGeometryComponent.h"


//
//	OtGeometryRenderData
//

class OtGeometryRenderData {
public:
	// analyze geometry and set render state
	void analyzeEntity(OtScene *scene, OtEntity entity);

	// analyze camera
	void analyzeCamera(size_t type, OtFrustum& frustum);

	// properties
	OtEntity entity;
	OtGeometryComponent* component;
	OtGeometry* geometry;
	int geometryVersion = 0;
	OtMaterial* material;
	glm::mat4 globalTransform;
	OtAABB worldAabb;
	bool changed;

	OtInstances* instances = nullptr;

	struct CameraView {
		OtFrustum frustum;
		bool visible;
		int instancesVersion = 0;
		std::vector<glm::mat4> visibleInstances;
	};

	std::array<CameraView, OtCascadedShadowMap::maxCascades + 3> cameras;

	// miscellaneous
	static inline OtMaterial dummyMaterial;
};
