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
	void renderPanel(const std::string& name, bool canAdd, std::function<void()> menu, std::function<void()> content);

	template<typename T, typename R>
	void renderComponent(const std::string& name, R render);

	// file actions
	void run();

	// the scene being edited
	OtScene2 scene;
	OtEntity selectedEntity;

	// work variables
	float panelWidth = -1;
	float spaceAvailable;
	float lineHeight;

	bool guizmoVisible = false;
	ImGuizmo::OPERATION guizmoOperation = ImGuizmo::TRANSLATE;

	bool guizmoSnapping = false;
	glm::vec3 snap = glm::vec3(1.0);
};
