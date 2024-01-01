//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>
#include <memory>

#include "glm/glm.hpp"
#include "ImGuizmo.h"

#include "OtScene.h"
#include "OtSceneEditorCamera.h"
#include "OtSceneRenderer.h"

#include "OtEditor.h"
#include "OtTaskManager.h"


//
//	OtSceneEditor
//

class OtSceneEditor : public OtEditor {
public:
	// constructor
	OtSceneEditor();

	// get file extension
	inline std::string getFileExtension() override { return ".ots"; }

	// load/save content
	void load() override;
	void save() override;

	// is the editor's content "dirty" (unsaved);
	bool isDirty() override;

	// clipboard operations
	void cutEntity();
	void copyEntity();
	void pasteEntity();
	void duplicateEntity();

	// create a new object
	static std::shared_ptr<OtSceneEditor> create(const std::filesystem::path& path);

private:
	// switch scene camera
	void setSceneCamera(int cameraNumber);

	// render the parts
	void renderMenu() override;
	void renderEditor(bool active) override;
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
	void renderComponent();

	template<typename... T>
	void renderComponents() {
		(renderComponent<T>(), ...);
	}

	// handle keyboard shortcuts
	void handleShortcuts();

	// the scene being edited
	std::unique_ptr<OtScene> scene;
	std::unique_ptr<OtSceneRenderer> renderer;
	OtEntity selectedEntity = OtEntityNull;
	OtEntity renamingEntity = OtEntityNull;

	// to handle do/undo/redo
	OtTaskManager taskManager;
	std::shared_ptr<OtEditorTask> nextTask = nullptr;

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

	// camera information
	OtEntity selectedCamera = OtEntityNull;
	OtSceneEditorCamera editorCamera;

	// grid controls
	bool gridEnabled = false;
	float gridScale = 1.0f;

	// guizmo controls
	bool guizmoEnabled = false;
	ImGuizmo::OPERATION guizmoOperation = ImGuizmo::TRANSLATE;
	bool guizmoSnapping = false;
	glm::vec3 guizmoSnapInterval = glm::vec3(0.1f);
};
