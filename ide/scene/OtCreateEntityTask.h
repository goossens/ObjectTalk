//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
	enum class Type {
		empty,
		camera,
		directionalLight,
		model,
		geometry,
		sky,
		skybox,
		terrain,
		water,
		particles
	};

	// constructor
	OtCreateEntityTask(OtScene* s, OtEntity p, Type t) : scene(s), type(t) {
		parentUuid = scene->getUuidFromEntity(p);
	}

	// get task name
	std::string name() { return "create entity"; }

	// do action
	void perform() override {
		// create an empty entity
		entity = scene->createEntity();
		entityUuid = scene->getUuidFromEntity(entity);
		scene->addEntityToParent(scene->getEntityFromUuid(parentUuid), entity);
		addComponents();
	}

	// undo action
	void undo() override {
		scene->removeEntity(entity);
	}

	// redo action
	void redo() override {
		// restore entity with previous UUID
		entity = scene->createEntity(entityUuid);
		scene->addEntityToParent(scene->getEntityFromUuid(parentUuid), entity);
		addComponents();
	}

	// get the created entity
	OtEntity getEntity() { return entity; }

private:
	// properties
	OtScene* scene;
	uint32_t parentUuid;
	Type type;

	OtEntity entity;
	uint32_t entityUuid;

	void addComponents() {
		// add components based on type
		switch (type) {
			case Type::empty:
				break;

			case Type::camera:
				scene->addComponent<OtCameraComponent>(entity);
				scene->addComponent<OtTransformComponent>(entity);
				break;

			case Type::directionalLight:
				scene->addComponent<OtDirectionalLightComponent>(entity);
				break;

			case Type::model:
				scene->addComponent<OtModelComponent>(entity);
				scene->addComponent<OtTransformComponent>(entity);
				break;

			case Type::geometry:
				scene->addComponent<OtGeometryComponent>(entity);
				scene->addComponent<OtTransformComponent>(entity);
				scene->addComponent<OtMaterialComponent>(entity);
				break;

			case Type::sky:
				scene->addComponent<OtSkyComponent>(entity);
				break;

			case Type::skybox:
				scene->addComponent<OtSkyBoxComponent>(entity);
				break;

			case Type::terrain:
				scene->addComponent<OtTerrainComponent>(entity);
				break;

			case Type::water:
				scene->addComponent<OtWaterComponent>(entity);
				break;

			case Type::particles:
				scene->addComponent<OtParticlesComponent>(entity);
				scene->addComponent<OtTransformComponent>(entity);
				break;
		}
	}
};
