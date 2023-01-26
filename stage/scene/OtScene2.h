//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "nlohmann/json_fwd.hpp"

#include "OtEcs.h"

#include "OtTransformComponent.h"
#include "OtCameraComponent.h"
#include "OtGeometryComponent.h"
#include "OtWorld.h"


//
//	Scene component lists
//

#define OtSceneAddableComponents \
	OtTransformComponent, \
	OtCameraComponent, \
	OtGeometryComponent

#define OtSceneRenderableComponents \
	OtTagComponent, \
	OtSceneAddableComponents

#define OtSceneSaveableComponents \
	OtUuidComponent, \
	OtSceneRenderableComponents

#define OtSceneAllComponents \
	OtHierarchyComponent, \
	OtSceneSaveableComponents


//
//	OtScene2
//

class OtScene2Class;
typedef std::shared_ptr<OtScene2Class> OtScene2;

class OtScene2Class : public OtWorldClass, public OtEcs {
public:
	// (de)serialize from/to string
	std::string serialize(int indent=-1, char character=' ');
	void deserialize(const std::string& data);

	std::string serializeEntity(OtEntity entity, int indent=-1, char character=' ');
	OtEntity deserializeEntity(const std::string& data);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtScene2 create();

private:
	// (de)serialize from/to JSON
	nlohmann::json serializeEntityToJson(OtEntity entity);
	OtEntity deserializeEntityFromJson(nlohmann::json& data);
};
