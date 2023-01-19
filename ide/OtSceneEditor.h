//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "ImGuizmo.h"

#include "OtScene2.h"

#include "OtEditor.h"


//
//	OtSceneEditorClass
//

class OtSceneEditorClass;
typedef std::shared_ptr<OtSceneEditorClass> OtSceneEditor;

class OtSceneEditorClass : public OtEditorClass {
public:
	// get file extension
	std::string getFileExtension() override { return ".ots"; }

	// load/save content
	void load() override;
	void save() override;

	// render the editor
	void render() override;

	// is the editor's content "dirty" (unsaved);
	bool isDirty() override;

	// create a new object
	static OtSceneEditor create(const std::string& filename);

private:
	// render the parts
	void renderMenu();
	void renderPanels();
	void renderEntitiesPanel();
	void renderComponentsPanel();
	void renderScene();

	// rendering helpers
	void determinePanelSizes();
	void renderPanel(const std::string& name, bool canAdd, std::function<void()> menu, std::function<void()> content);
	void renderEntity(OtEntity entity);
	void renderChildEntities(OtEntity entity);
	void renderNewEntitiesMenu(OtEntity entity);

	template<typename T>
	void renderComponent(const std::string& name);

	// file actions
	void run();

	// the scene being edited
	OtScene2 scene;
	OtEntity selectedEntity = OtNullEntity;

	// work variables
	float panelWidth = -1;
	float minPanelWidth = -1;
	float maxPanelWidth = -1;
	float entityPanelHeight = -1;
	float minEntityPanelHeight = -1;
	float maxEntityPanelHeight = -1;
	float buttonSize;

	bool guizmoVisible = false;
	ImGuizmo::OPERATION guizmoOperation = ImGuizmo::TRANSLATE;

	bool guizmoSnapping = false;
	glm::vec3 snap = glm::vec3(1.0);

	// immediate mode GUIs make it difficult to change hierarchies
	// while there being rendered which is why we collect all
	// deletions and moves here so they can be processed at the end
	// of the current frame
	OtEntity entityToBeRemoved = OtNullEntity;
	OtEntity entityToBeMoved = OtNullEntity;
	OtEntity entityToBeMovedBefore = OtNullEntity;
	OtEntity entityToBeMovedInto = OtNullEntity;
};
