//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <string>

#include "OtEditorTask.h"
#include "OtScene.h"


//
//	OtSaveEditorCameraTask
//

class OtSaveEditorCameraTask : public OtEditorTask {
public:
	// constructor
	OtSaveEditorCameraTask(
		OtScene* s,
		OtEntity e,
		const std::string& oc, const std::string& ot,
		const std::string& nc, const std::string& nt) :
			scene(s),
			oldCamera(oc), oldTransform(ot),
			newCamera(nc), newTransform(nt) {

		entityUuid = scene->getUuidFromEntity(e);
	}

	// get task name
	std::string name() { return "save editor camera to"; }

	// do action
	void perform() override {
		// nothing to be done (as it already happened)
	}

	// undo action
	void undo() override {
		// restore old value
		auto entity = scene->getEntityFromUuid(entityUuid);

		auto& cameraComponent = scene->getComponent<OtCameraComponent>(entity);
		auto cameraData = nlohmann::json::parse(oldCamera);
		cameraComponent.deserialize(cameraData, nullptr);

		auto& transformComponent = scene->getComponent<OtTransformComponent>(entity);
		auto transformData = nlohmann::json::parse(oldTransform);
		transformComponent.deserialize(transformData, nullptr);
	}

	// redo action
	void redo() override {
		// restore old value
		auto entity = scene->getEntityFromUuid(entityUuid);

		auto& cameraComponent = scene->getComponent<OtCameraComponent>(entity);
		auto cameraData = nlohmann::json::parse(newCamera);
		cameraComponent.deserialize(cameraData, nullptr);

		auto& transformComponent = scene->getComponent<OtTransformComponent>(entity);
		auto transformData = nlohmann::json::parse(newTransform);
		transformComponent.deserialize(transformData, nullptr);
	}

private:
	// properties
	OtScene* scene;
	uint32_t entityUuid;
	std::string oldCamera;
	std::string oldTransform;
	std::string newCamera;
	std::string newTransform;
};
