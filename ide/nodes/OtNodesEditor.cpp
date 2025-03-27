//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "imgui.h"
#include "imgui_internal.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtNodesEditor.h"

#include "OtAlignNodesTask.h"
#include "OtCopyNodesTask.h"
#include "OtChangeLinkTask.h"
#include "OtCreateLinkTask.h"
#include "OtCreateNodeTask.h"
#include "OtCutNodesTask.h"
#include "OtDeleteLinkTask.h"
#include "OtDeleteNodesTask.h"
#include "OtDuplicateNodesTask.h"
#include "OtDragNodesTask.h"
#include "OtEditNodeTask.h"
#include "OtPasteNodesTask.h"


//
//	OtNodesEditor::clipboard
//

std::string OtNodesEditor::clipboard;


//
//	OtNodesEditor::clear
//

void OtNodesEditor::clear() {
	nodes.clear();
}


//
//	OtNodesEditor::load
//

void OtNodesEditor::load() {
	nodes.load(path);
}


//
//	OtNodesEditor::saveFile
//

void OtNodesEditor::save() {
	nodes.save(path);
	taskManager.baseline();
}


//
//	OtNodesEditor::cutSelectedNodes
//

void OtNodesEditor::cutSelectedNodes() {
	nextTask = std::make_shared<OtCutNodesTask>(&nodes, clipboard);
}


//
//	OtNodesEditor::copySelectedNodes
//

void OtNodesEditor::copySelectedNodes() {
	nextTask = std::make_shared<OtCopyNodesTask>(&nodes, clipboard);
}


//
//	OtNodesEditor::pasteSelectedNodes
//

void OtNodesEditor::pasteSelectedNodes() {
	nextTask = std::make_shared<OtPasteNodesTask>(&nodes, clipboard);
}


//
//	OtNodesEditor::deleteSelectedNodes
//

void OtNodesEditor::deleteSelectedNodes() {
	nextTask = std::make_shared<OtDeleteNodesTask>(&nodes);
}


//
//	OtNodesEditor::duplicateSelectedNodes
//

void OtNodesEditor::duplicateSelectedNodes() {
	nextTask = std::make_shared<OtDuplicateNodesTask>(&nodes);
}


//
//	OtNodesEditor::alignSelectedNodes
//

void OtNodesEditor::alignSelectedNodes(OtAlignNodesTask::Alignment alignment) {
	nextTask = std::make_shared<OtAlignNodesTask>(&nodes, alignment);
}


//
//	OtNodesEditor::renderMenus
//

void OtNodesEditor::renderMenus() {
	// get status
	bool selected = nodes.hasSelected();
	bool multipleSelected = nodes.hasMultipleSelected();

	if (ImGui::BeginMenu("Edit")) {
		if (ImGui::MenuItem("Undo", OT_UI_SHORTCUT "Z", nullptr, taskManager.canUndo())) { taskManager.undo(); }
#if __APPLE__
		if (ImGui::MenuItem("Redo", "^" OT_UI_SHORTCUT "Z", nullptr, taskManager.canRedo())) { taskManager.redo(); }
#else
		if (ImGui::MenuItem("Redo", OT_UI_SHORTCUT "Y", nullptr, taskManager.canRedo())) { taskManager.redo(); }
#endif

		ImGui::Separator();
		if (ImGui::MenuItem("Cut", OT_UI_SHORTCUT "X", nullptr, selected)) { cutSelectedNodes(); }
		if (ImGui::MenuItem("Copy", OT_UI_SHORTCUT "C", nullptr, selected)) { copySelectedNodes(); }
		if (ImGui::MenuItem("Paste", OT_UI_SHORTCUT "V", nullptr, clipboard.size() > 0)) { pasteSelectedNodes(); }
		if (ImGui::MenuItem("Delete", "Del", nullptr, selected)) { deleteSelectedNodes(); }
		if (ImGui::MenuItem("Duplicate", OT_UI_SHORTCUT "D", nullptr, selected)) { duplicateSelectedNodes(); }
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Selection")) {
		if (ImGui::MenuItem("Select All", OT_UI_SHORTCUT "A")) { nodes.selectAll(); }
		if (ImGui::MenuItem("Clear Selection")) { nodes.deselectAll(); }
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Align")) {
		if (ImGui::MenuItem("Left", nullptr, nullptr, multipleSelected)) { alignSelectedNodes(OtAlignNodesTask::Alignment::left); }
		if (ImGui::MenuItem("Center", nullptr, nullptr, multipleSelected)) { alignSelectedNodes(OtAlignNodesTask::Alignment::center); }
		if (ImGui::MenuItem("Right", nullptr, nullptr, multipleSelected)) { alignSelectedNodes(OtAlignNodesTask::Alignment::right); }
		ImGui::Separator();
		if (ImGui::MenuItem("Top", nullptr, nullptr, multipleSelected)) { alignSelectedNodes(OtAlignNodesTask::Alignment::top); }
		if (ImGui::MenuItem("Middle", nullptr, nullptr, multipleSelected)) { alignSelectedNodes(OtAlignNodesTask::Alignment::middle); }
		if (ImGui::MenuItem("Bottom", nullptr, nullptr, multipleSelected)) { alignSelectedNodes(OtAlignNodesTask::Alignment::bottom); }
		ImGui::EndMenu();
	}
}


//
//	OtNodesEditor::handleShortcuts
//

void OtNodesEditor::handleShortcuts() {
	// get status
	bool selected = nodes.hasSelected();

	if (ImGui::IsKeyDown(ImGuiMod_Ctrl)) {
		if (ImGui::IsKeyDown(ImGuiMod_Shift) && ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
			if (taskManager.canRedo()) {
				taskManager.redo();
			}

		} else if (ImGui::IsKeyPressed(ImGuiKey_Z, false) && taskManager.canUndo()) {
			// this is a hack as Dear ImGui's InputText keeps a private copy of its content
			// ClearActiveID() takes the possible focus away and allows undo to work
			// see ImGuiInputTextFlags_NoUndoRedo documentation in imgui.h
			// so much for immediate mode :-)
			ImGui::ClearActiveID();
			taskManager.undo();

		} else if (ImGui::IsKeyPressed(ImGuiKey_X, false) && selected) {
			cutSelectedNodes();

		} else if (ImGui::IsKeyPressed(ImGuiKey_C, false) && selected) {
			copySelectedNodes();

		} else if (ImGui::IsKeyPressed(ImGuiKey_V, false) && clipboard.size() > 0) {
			pasteSelectedNodes();

		} else if (ImGui::IsKeyPressed(ImGuiKey_D, false) && selected) {
			duplicateSelectedNodes();

		} else if (ImGui::IsKeyPressed(ImGuiKey_A, false)) {
			nodes.selectAll();
		}

	} else if ((ImGui::IsKeyPressed(ImGuiKey_Backspace, false) || ImGui::IsKeyPressed(ImGuiKey_Delete, false)) && selected) {
		deleteSelectedNodes();
	}
}


//
//	OtNodesEditor::renderEditor
//

void OtNodesEditor::renderEditor() {
	// evaluate the nodes
	nodes.evaluate();

	// render the nodes
	widget.render(&nodes);

	// handle node editing events
	uint32_t node;

	if (widget.isNodeEdited(node)) {
		nextTask = std::make_shared<OtEditNodeTask>(&nodes, node);
	}

	// handle link creations
	uint32_t from;
	uint32_t to;

	if (widget.isCreatingLink(from, to)) {
		nextTask = std::make_shared<OtCreateLinkTask>(&nodes, from, to);
	}

	// handle link deletion
	if (widget.isDroppingLink(from, to)) {
		nextTask = std::make_shared<OtDeleteLinkTask>(&nodes, from, to);
	}

	// handle link redirection
	uint32_t newTo;

	if (widget.isChangingLink(from, to, newTo)) {
		nextTask = std::make_shared<OtChangeLinkTask>(&nodes, from, to, newTo);
	}

	// handle node dragging
	ImVec2 offset;

	if (widget.isDraggingComplete(offset)) {
		nextTask = std::make_shared<OtDragNodesTask>(&nodes, offset);
	}

	// handle context menu
	if (widget.isRequestingContextMenu()) {
		ImGui::OpenPopup("Background Context");
	}

	if (ImGui::BeginPopup("Background Context")) {
		auto nodePosition = widget.getContextMenuPos();
		ImGui::TextUnformatted("Create Node");
		ImGui::Separator();

		OtNodesFactory::eachCategory([&](OtNodeCategory& category) {
			if (ImGui::BeginMenu(category.name.c_str())) {
				category.eachType([&](const std::string& type) {
					if (ImGui::MenuItem(type.c_str())) {
						nextTask = std::make_shared<OtCreateNodeTask>(
							&nodes,
							type,
							nodePosition.x,
							nodePosition.y);
					}
				});

				ImGui::EndMenu();
			}
		});

		ImGui::EndPopup();
	}

	// perform editing task (if required)
	if (nextTask) {
		taskManager.perform(nextTask);
		nextTask = nullptr;
	}
}
