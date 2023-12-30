//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <vector>

#include "imgui.h"

#include "OtGraph.h"

#include "OtEditor.h"
#include "OtEditorTask.h"
#include "OtGraphWidget.h"
#include "OtTaskManager.h"


//
//	OtGraphEditor
//

class OtGraphEditor : public OtEditor {
public:
	// constructor
	OtGraphEditor();

	// get file extension
	inline std::string getFileExtension() override { return ".otg"; }

	// load/save content
	void load() override;
	void save() override;

	// render the editor
	void renderMenu() override;
	void renderEditor(bool active) override;

	// is the editor's content "dirty" (unsaved);
	bool isDirty() override;

	// clipboard operations
	void cutSelectedNodes();
	void copySelectedNodes();
	void pasteSelectedNodes();
	void deleteSelectedNodes();
	void duplicateSelectedNodes();

	// create a new object
	static std::shared_ptr<OtGraphEditor> create(const std::filesystem::path& path);

private:
	// the graph being edited and the editor's UI
	std::unique_ptr<OtGraph> graph;
	std::unique_ptr<OtGraphWidget> widget;

	// to handle do/undo/redo
	OtTaskManager taskManager;
	std::shared_ptr<OtEditorTask> nextTask = nullptr;

	// to handle cut/copy/paste
	std::string clipboard;
};
