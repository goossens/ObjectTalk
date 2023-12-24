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
#include "imgui_node_editor.h"

#include "OtGraph.h"

#include "OtEditor.h"
#include "OtTaskManager.h"


//
//	OtGraphEditor
//

class OtGraphEditor : public OtEditor {
public:
	// constructor
	OtGraphEditor();

	// get file extension
	std::string getFileExtension() override { return ".otg"; }

	// load/save content
	void load() override;
	void save() override;

	// is the editor's content "dirty" (unsaved);
	bool isDirty() override;

	// clipboard operations
	void cutNodes();
	void copyNodes();
	void pasteNodes();
	void duplicateNodes();

	// create a new object
	static std::shared_ptr<OtGraphEditor> create(const std::filesystem::path& path);

private:
	// render the parts
	void renderMenu() override;
	void renderEditor(bool active) override;
	void renderNode(OtGraphNode& node);
	void renderPin(OtGraphPin& pin, float x);
	void handleInteractions();

	// the graph being edited and the editor's UI
	std::unique_ptr<OtGraph> graph;
	ax::NodeEditor::EditorContext* editorContext = nullptr;

	// to handle do/undo/redo
	OtTaskManager taskManager;
	std::shared_ptr<OtEditorTask> nextTask = nullptr;

	// to handle cut/copy/paste
	std::string clipboard;

	// currently selected nodes
	std::vector<int> selectedNodes;

	// work variables
	bool firstFrame = true;
	ImVec4 nodePadding;
	float nodeBorderWidth;
};
