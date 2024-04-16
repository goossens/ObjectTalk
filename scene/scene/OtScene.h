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
#include "OtGeometryComponent.h"
#include "OtIblComponent.h"
#include "OtInstancingComponent.h"
#include "OtMaterialComponent.h"
#include "OtModelComponent.h"
#include "OtNodesComponent.h"
#include "OtScriptComponent.h"
#include "OtSkyBoxComponent.h"
#include "OtPostProcessingComponent.h"
#include "OtSkyComponent.h"
#include "OtParticlesComponent.h"
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
	OtIblComponent, \
	OtInstancingComponent, \
	OtMaterialComponent, \
	OtCameraComponent, \
	OtDirectionalLightComponent, \
	OtScriptComponent, \
	OtNodesComponent, \
	OtBackgroundComponent, \
	OtTerrainComponent, \
	OtSkyComponent, \
	OtSkyBoxComponent, \
	OtParticlesComponent, \
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

	// archive/restore entities to/from a string (support for do/undo/redo operations)
	std::string archiveEntity(OtEntity entity, int indent=-1, char character=' ', std::string* basedir=nullptr);
	OtEntity restoreEntity(const std::string& data, std::string* basedir=nullptr);
	OtEntity duplicateEntity(const std::string& data, std::string* basedir=nullptr);

	// get the composite worldspace transform for the specified entity
	glm::mat4 getGlobalTransform(OtEntity entity);

	// post processing access
	inline OtPostProcessingComponent& getPostProcessing() { return postProcessing; }

	// evaluate all nodes
	void evaluateNodes();

private:
	// metadata for editor
	std::string metadata{"{}"};

	// post processing settings
	OtPostProcessingComponent postProcessing;

	// (de)serialize from/to JSON
	nlohmann::json serializeEntityToJson(OtEntity entity, std::string* basedir);
	OtEntity deserializeEntityFromJson(nlohmann::json& json, std::string* basedir, bool preserveUuid);
};
