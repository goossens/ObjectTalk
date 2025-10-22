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
#include "OtCamera.h"
#include "OtCascadedShadowMap.h"
#include "OtFrustum.h"
#include "OtInstances.h"
#include "OtModel.h"

#include "OtScene.h"

#include "OtModelComponent.h"


//
//	OtModelRenderData
//

class OtModelRenderData {
public:
	// analyze geometry and set render state
	void analyzeEntity(OtScene *scene, OtEntity entity);

	// analyze camera
	void analyzeCamera(size_t type, OtCamera& camera);

	// properties
	OtEntity entity;
	OtModelComponent* component;
	OtModel* model;
	glm::mat4 globalTransform;
	OtAABB worldAabb;
	bool visible[OtCascadedShadowMap::maxCascades + 3];
};
