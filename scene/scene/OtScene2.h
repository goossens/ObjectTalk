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

#include "nlohmann/json_fwd.hpp"

#include "OtObject.h"

#include "OtEcs.h"

#include "OtBackgroundComponent.h"
#include "OtCameraComponent.h"
#include "OtGeometryComponent.h"
#include "OtMaterialComponent.h"
#include "OtSkyBoxComponent.h"
#include "OtPostProcessingComponent.h"
#include "OtSkySphereComponent.h"
#include "OtTransformComponent.h"


//
//	Scene component lists
//

#define OtSceneComponents \
	OtTransformComponent, \
	OtGeometryComponent, \
	OtMaterialComponent, \
	OtCameraComponent, \
	OtBackgroundComponent, \
	OtSkyBoxComponent, \
	OtSkySphereComponent, \
	OtPostProcessingComponent


//
//	OtScene2
//

class OtScene2Class;
typedef std::shared_ptr<OtScene2Class> OtScene2;

class OtScene2Class : public OtObjectClass, public OtEcs {
public:
	// (de)serialize from/to string
	std::string serialize(int indent=-1, char character=' ', std::filesystem::path* basedir=nullptr);
	void deserialize(const std::string& data, std::filesystem::path* basedir=nullptr);

	std::string serializeEntity(OtEntity entity, int indent=-1, char character=' ', std::filesystem::path* basedir=nullptr);
	OtEntity deserializeEntity(const std::string& data, std::filesystem::path* basedir=nullptr);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtScene2 create();

private:
	// (de)serialize from/to JSON
	nlohmann::json serializeEntityToJson(OtEntity entity, std::filesystem::path* basedir);
	OtEntity deserializeEntityFromJson(nlohmann::json& data, std::filesystem::path* basedir);
};
