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

#include "ImGuizmo.h"

#include "OtOrbitalCamera.h"
#include "OtScene2.h"
#include "OtSceneRenderer.h"

#include "OtEditor.h"
#include "OtTaskManager.h"


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

	// clipboard operations
	void cutEntity();
	void copyEntity();
	void pasteEntity();
	void duplicateEntity();

	// create a new object
	static OtSceneEditor create(const std::filesystem::path& path);

private:
	// render the parts
	void renderMenu();
	void renderPanels();
	void renderEntitiesPanel();
	void renderComponentsPanel();
	void renderViewPort();

	// rendering helpers
	void determinePanelSizes();
	void renderPanel(const std::string& name, bool canAdd, std::function<void()> menu, std::function<void()> content);
	void renderEntity(OtEntity entity);
	void renderChildEntities(OtEntity entity);
	void renderNewEntitiesMenu(OtEntity entity);

	template <typename T>
	void renderNewComponent();

	template<typename... T>
	void renderNewComponents() {
		(renderNewComponent<T>(), ...);
	}

	template<typename T>
	void renderComponentEditor();

	template<typename T>
	void renderComponent(bool canRemove=true);

	template<typename... T>
	void renderComponents() {
		(renderComponent<T>(), ...);
	}

	// handle keyboard shortcuts
	void handleShortcuts();

	// file actions
	void run();

	// the scene being edited
	OtScene2 scene;
	OtEntity selectedEntity = OtEntityNull;
	OtSceneRenderer renderer;

	// to handle do/undo/redo
	OtTaskManager taskManager;
	std::shared_ptr<OtEditorTask> nextTask = nullptr;
	size_t version = 0;

	// to handle cut/copy/paste
	std::string clipboard;

	// work variables
	float panelWidth = -1.0f;
	float minPanelWidth = -1.0f;
	float maxPanelWidth = -1.0f;
	float entityPanelHeight = -1.0f;
	float minEntityPanelHeight = -1.0f;
	float maxEntityPanelHeight = -1.0f;
	float buttonSize;

	OtOrbitalCamera editorCamera;

	bool guizmoVisible = false;
	ImGuizmo::OPERATION guizmoOperation = ImGuizmo::TRANSLATE;

	bool guizmoSnapping = false;
	glm::vec3 snap = glm::vec3(0.1f);
};
