//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>
#include <string>

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"

#include "OtEcs.h"

#include "OtBackgroundComponent.h"
#include "OtCameraComponent.h"
#include "OtDirectionalLightComponent.h"
#include "OtGeometryComponent.h"
#include "OtInstancingComponent.h"
#include "OtMaterialComponent.h"
#include "OtModelComponent.h"
#include "OtScriptComponent.h"
#include "OtSkyBoxComponent.h"
#include "OtPostProcessingComponent.h"
#include "OtSkyComponent.h"
#include "OtSkySphereComponent.h"
#include "OtTerrainComponent.h"
#include "OtTransformComponent.h"
#include "OtWaterComponent.h"


//
//	Scene component lists
//

#define OtSceneComponents \
	OtGeometryComponent, \
	OtModelComponent, \
	OtTransformComponent, \
	OtInstancingComponent, \
	OtMaterialComponent, \
	OtCameraComponent, \
	OtDirectionalLightComponent, \
	OtScriptComponent, \
	OtBackgroundComponent, \
	OtTerrainComponent, \
	OtSkyComponent, \
	OtSkyBoxComponent, \
	OtSkySphereComponent, \
	OtPostProcessingComponent, \
	OtWaterComponent


//
//	OtScene
//

class OtScene : public OtEcs {
public:
	// load and save scene
	void load(const std::filesystem::path& path, nlohmann::json* metadata=nullptr);
	void save(const std::filesystem::path& path, nlohmann::json* metadata=nullptr);

	// (de)serialize entiry from/to string
	std::string serializeEntity(OtEntity entity, int indent=-1, char character=' ', std::filesystem::path* basedir=nullptr);
	OtEntity deserializeEntity(const std::string& data, std::filesystem::path* basedir=nullptr);

	// get the composite worldspace transform for the specified entity
	glm::mat4 getGlobalTransform(OtEntity entity);

private:
	// (de)serialize from/to JSON
	nlohmann::json serializeEntityToJson(OtEntity entity, std::filesystem::path* basedir);
	OtEntity deserializeEntityFromJson(nlohmann::json& data, std::filesystem::path* basedir);
};
