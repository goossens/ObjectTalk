//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

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

	void renderVec3Control(const std::string& label, glm::vec3& value, float defaultValue=0.0);

	// file actions
	void run();

	// the scene being edited
	OtScene2 scene;
	OtEntity selectedEntity;

	//	properties
	float panelWidth = -1;
	float spaceAvailable;
	float lineHeight;
};
