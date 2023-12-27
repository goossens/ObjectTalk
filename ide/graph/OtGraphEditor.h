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
#include "OtGraphEditorTask.h"
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

	// render the editor
	inline void startRender() override { ax::NodeEditor::SetCurrentEditor(editorContext); }
	void renderMenu() override;
	void renderEditor(bool active) override;
	inline void endRender() override { ax::NodeEditor::SetCurrentEditor(nullptr); }

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
	// render the parts
	void renderNode(OtGraphNode& node);
	void renderPin(OtGraphPin& pin, float x);

	// handle graph interactions
	void handleInteractions();

	// see if nodes are selected
	bool areNodesSelected();
	std::vector<uint32_t> getSelectedNodes(uint32_t id=0);

	// the graph being edited and the editor's UI
	std::unique_ptr<OtGraph> graph;
	ax::NodeEditor::EditorContext* editorContext = nullptr;

	// to handle do/undo/redo
	OtTaskManager taskManager;
	std::shared_ptr<OtGraphEditorTask> nextTask = nullptr;
	std::vector<uint32_t> nextSelection;

	// to handle cut/copy/paste
	std::string clipboard;

	// work variables
	ImVec4 nodePadding;
	float nodeBorderWidth;
	uint32_t draggedNodeId = 0;
};
