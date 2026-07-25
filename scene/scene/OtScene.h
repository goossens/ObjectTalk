//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "glm/glm.hpp" // IWYU pragma: keep

#include "OtEcs.h"

#include "OtBackgroundComponent.h" // IWYU pragma: keep
#include "OtCameraComponent.h" // IWYU pragma: keep
#include "OtDirectionalLightComponent.h" // IWYU pragma: keep
#include "OtGeometryComponent.h" // IWYU pragma: keep
#include "OtGrassComponent.h" // IWYU pragma: keep
#include "OtIblComponent.h" // IWYU pragma: keep
#include "OtInstancingComponent.h" // IWYU pragma: keep
#include "OtMaterialComponent.h" // IWYU pragma: keep
#include "OtMessageComponent.h" // IWYU pragma: keep
#include "OtModelComponent.h" // IWYU pragma: keep
#include "OtNodesComponent.h" // IWYU pragma: keep
#include "OtScriptComponent.h" // IWYU pragma: keep
#include "OtSkyBoxComponent.h" // IWYU pragma: keep
#include "OtPointLightComponent.h" // IWYU pragma: keep
#include "OtPostProcessingComponent.h" // IWYU pragma: keep
#include "OtSkyComponent.h" // IWYU pragma: keep
#include "OtParticlesComponent.h" // IWYU pragma: keep
#include "OtTerrainComponent.h" // IWYU pragma: keep
#include "OtTransformComponent.h" // IWYU pragma: keep
#include "OtWaterComponent.h" // IWYU pragma: keep


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
	OtIblComponent, \
	OtDirectionalLightComponent, \
	OtPointLightComponent, \
	OtScriptComponent, \
	OtNodesComponent, \
	OtBackgroundComponent, \
	OtTerrainComponent, \
	OtSkyComponent, \
	OtSkyBoxComponent, \
	OtParticlesComponent, \
	OtWaterComponent, \
	OtGrassComponent, \
	OtMessageComponent


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

	// get the composite world space transform for the specified entity
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
