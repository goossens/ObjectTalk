//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <string>

#include "glm/glm.hpp"
#include "ImGuizmo.h"

#include "OtScene.h"
#include "OtSceneAsset.h"
#include "OtSceneEditorCamera.h"
#include "OtSceneRenderer.h"
#include "OtSceneRendererDebug.h"

#include "OtEditor.h"
#include "OtTaskManager.h"


//
//	OtSceneEditor
//

class OtSceneEditor : public OtEditor {
public:
	// constructor
	OtSceneEditor();

	// file handling functions
	void newFile(const std::string& path) override;
	void openFile(const std::string& path) override;
	void saveFile() override;
	void saveAsFile(const std::string& path) override;
	inline std::string getFileExtension() override { return ".ots"; }
	inline std::string getFilePath() override { return asset.getPath(); }

	// get editor status
	inline bool isReady() override { return asset.isReady(); }
	bool isDirty() override;

	// clipboard operations
	void cutEntity();
	void copyEntity();
	void pasteEntity();
	void duplicateEntity();

private:
	// metadata methods
	void processMetaData();
	void generateMetaData();

	// switch scene camera
	void setSceneCamera(int cameraNumber);

	// render the parts
	void renderMenu() override;
	void renderEditor() override;
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

	// the scene being edited
	OtAsset<OtSceneAsset> asset;
	OtAssetPostLoadListerner loadListener;

	// a renderer to show the scene
	std::unique_ptr<OtSceneRenderer> renderer;

	// a debug window for the renderer
	std::unique_ptr<OtSceneRendererDebug> renderDebug;
	bool showRendererDebug = false;
	void toggleRendererDebug();

	// tracking entities for certain tasks
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
