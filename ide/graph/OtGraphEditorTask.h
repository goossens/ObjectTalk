//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_node_editor.h"

#include "OtEditorTask.h"
#include "OtGraph.h"


//
//	OtGraphEditorTask
//

class OtGraphEditorTask : public OtEditorTask {
public:
	// delete nodes in editor
	void deleteNodesInEditor(const std::vector<uint32_t>& nodes) {
		for (auto node : nodes) {
			ax::NodeEditor::DeleteNode(node);
		}
	}

	// get list of selected nodes in editor
	std::vector<uint32_t> getSelectedNodesInEditor() {
		std::vector<ax::NodeEditor::NodeId> nodes;
		nodes.resize(ax::NodeEditor::GetSelectedNodes(nullptr, 0));
		size_t selected = ax::NodeEditor::GetSelectedNodes(nodes.data(), nodes.size());
		std::vector<uint32_t> result;

		for (auto i = 0; i < selected; i++) {
			result.push_back((uint32_t) nodes[i].Get());
		}

		return result;
	}

	// clear selection in editor
	void clearEditorSelection() {
		ax::NodeEditor::ClearSelection();
	}

	// select specified nodes in editor
	void selectNodesInEditor(const std::vector<uint32_t>& nodes) {
		newSelection = true;
		selection = nodes;
	}

	// do we have a new selection
	bool hasNewSelection() {
		return newSelection;
	}

	// get new selection
	std::vector<uint32_t>& getNewSelection() {
		newSelection = false;
		return selection;
	}

private:
	bool newSelection = false;
	std::vector<uint32_t> selection;
};
