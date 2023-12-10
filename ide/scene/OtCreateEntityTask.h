//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtEditorTask.h"
#include "OtEntity.h"
#include "OtScene.h"


//
//	OtCreateEntityTask
//

class OtCreateEntityTask : public OtEditorTask {
public:
	// entity type
	enum Type {
		empty,
		camera,
		directionalLight,
		pointLight,
		model,
		geometry,
		terrain,
		water
	};

	// constructor
	OtCreateEntityTask(OtScene* s, OtEntity p, Type t) : scene(s), type(t) {
		parentUuid = scene->getUuidFromEntity(p);
	}

	// get task name
	std::string name() { return "create entity"; }

	// perform action
	virtual void perform() {
		// create an empty entity (preserving UUID on redo)
		entity = scene->createEntity(entityUuid);
		entityUuid = scene->getUuidFromEntity(entity);
		scene->addEntityToParent(scene->getEntityFromUuid(parentUuid), entity);

		// add components based on type
		switch (type) {
			case empty:
				break;

			case camera:
				scene->addComponent<OtCameraComponent>(entity);
				scene->addComponent<OtTransformComponent>(entity);
				scene->addComponent<OtPostProcessingComponent>(entity);
				break;

			case directionalLight:
				scene->addComponent<OtDirectionalLightComponent>(entity);
				scene->addComponent<OtTransformComponent>(entity);
				break;

			case pointLight:
				break;

			case model:
				scene->addComponent<OtModelComponent>(entity);
				scene->addComponent<OtTransformComponent>(entity);
				break;

			case geometry:
				scene->addComponent<OtGeometryComponent>(entity);
				scene->addComponent<OtTransformComponent>(entity);
				scene->addComponent<OtMaterialComponent>(entity);
				break;

			case terrain:
				scene->addComponent<OtTerrainComponent>(entity);
				break;

			case water:
				scene->addComponent<OtWaterComponent>(entity);
				break;
		}
	}

	// undo action
	virtual void undo() {
		scene->removeEntity(entity);
	}

	// get the created entity
	OtEntity getEntity() { return entity; }

private:
	// properties
	OtScene* scene;
	uint32_t parentUuid;
	Type type;

	OtEntity entity;
	uint32_t entityUuid = 0;
};
