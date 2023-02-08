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
#include "OtScene2.h"


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
		geometry
	};

	// constructor
	OtCreateEntityTask(OtScene2 s, OtEntity p, Type t) : scene(s), type(t) {
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
				break;

			case directionalLight:
				break;

			case pointLight:
				break;

			case geometry:
				scene->addComponent<OtTransformComponent>(entity);
				scene->addComponent<OtGeometryComponent>(entity);
				scene->addComponent<OtMaterialComponent>(entity);
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
	OtScene2 scene;
	uint32_t parentUuid;
	Type type;

	OtEntity entity;
	uint32_t entityUuid = 0;
};
