//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "glm/glm.hpp"

#include "OtEcs.h"

#include "OtBackgroundComponent.h"
#include "OtCameraComponent.h"
#include "OtDirectionalLightComponent.h"
#include "OtFlowComponent.h"
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
	OtFlowComponent, \
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
	void load(const std::string& path);
	void save(const std::string& path);

	// clear a scene
	void clear();

	// access the metadata
	inline void setMetaData(const std::string& m) { metadata = m; }
	inline std::string& getMetaData() { return metadata; }

	// (de)serialize entiry from/to string
	std::string serializeEntity(OtEntity entity, int indent=-1, char character=' ', std::string* basedir=nullptr);
	OtEntity deserializeEntity(const std::string& data, std::string* basedir=nullptr);

	// get the composite worldspace transform for the specified entity
	glm::mat4 getGlobalTransform(OtEntity entity);

private:
	// metadata for editor
	std::string metadata{"{}"};

	// (de)serialize from/to JSON
	nlohmann::json serializeEntityToJson(OtEntity entity, std::string* basedir);
	OtEntity deserializeEntityFromJson(nlohmann::json& data, std::string* basedir);
};
