//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtGraphEditor.h"

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
//	OtGraphEditor::OtGraphEditor
//

OtGraphEditor::OtGraphEditor() {
	widget = std::make_unique<OtGraphWidget>();
}


//
//	OtGraphEditor::newFile
//

void OtGraphEditor::newFile(const std::string& path) {
	// setup the asset
	asset = path;
}


//
//	OtGraphEditor::openFile
//

void OtGraphEditor::openFile(const std::string& path) {
	// setup the asset
	asset.load(path);
}


//
//	OtGraphEditor::saveFile
//

void OtGraphEditor::saveFile() {
	asset.save();
	taskManager.baseline();
}


//
//	OtGraphEditor::saveAsFile
//

void OtGraphEditor::saveAsFile(const std::string& path) {
	asset.saveAs(path);
	taskManager.baseline();
}



//
//	OtGraphEditor::isDirty
//

bool OtGraphEditor::isDirty() {
	return taskManager.isDirty();
}


//
//	OtGraphEditor::cutSelectedNodes
//

void OtGraphEditor::cutSelectedNodes() {
	nextTask = std::make_shared<OtCutNodesTask>(asset->getGraph(), clipboard);
}


//
//	OtGraphEditor::copySelectedNodes
//

void OtGraphEditor::copySelectedNodes() {
	nextTask = std::make_shared<OtCopyNodesTask>(asset->getGraph(), clipboard);
}


//
//	OtGraphEditor::pasteSelectedNodes
//

void OtGraphEditor::pasteSelectedNodes() {
	nextTask = std::make_shared<OtPasteNodesTask>(asset->getGraph(), clipboard);
}


//
//	OtGraphEditor::deleteSelectedNodes
//

void OtGraphEditor::deleteSelectedNodes() {
	nextTask = std::make_shared<OtDeleteNodesTask>(asset->getGraph());
}


//
//	OtGraphEditor::duplicateSelectedNodes
//

void OtGraphEditor::duplicateSelectedNodes() {
	nextTask = std::make_shared<OtDuplicateNodesTask>(asset->getGraph());
}


//
//	OtGraphEditor::renderMenu
//

void OtGraphEditor::renderMenu() {
	// get status
	bool selected = asset->getGraph()->hasSelected();
	bool clipable = clipboard.size() > 0;

	// create menubar
	if (ImGui::BeginMenuBar()) {
		renderFileMenu();

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
			if (ImGui::MenuItem("Paste", OT_UI_SHORTCUT "V", nullptr, selected && clipable)) { pasteSelectedNodes(); }
			if (ImGui::MenuItem("Delete", "Del", nullptr, selected)) { deleteSelectedNodes(); }
			if (ImGui::MenuItem("Duplicate", OT_UI_SHORTCUT "D", nullptr, selected)) { duplicateSelectedNodes(); }
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	// handle keyboard shortcuts (if required)
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows) && ImGui::IsKeyDown(ImGuiMod_Shortcut)) {
		if (ImGui::IsKeyDown(ImGuiMod_Shift) && ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
			if (taskManager.canRedo()) {
				taskManager.redo();
			}

		} else if (ImGui::IsKeyPressed(ImGuiKey_Z, false) && taskManager.canUndo()) {
			// this is a hack as ImGui's InputText keeps a private copy of its content
			// ClearActiveID() takes the possible focus away and allows undo to work
			// see ImGuiInputTextFlags_NoUndoRedo documentation in imgui.h
			// so much for immediate mode :-)
			ImGui::ClearActiveID();
			taskManager.undo();

		} else if (ImGui::IsKeyPressed(ImGuiKey_X, false) && selected) {
			cutSelectedNodes();

		} else if (ImGui::IsKeyPressed(ImGuiKey_C, false) && selected) {
			copySelectedNodes();

		} else if (ImGui::IsKeyPressed(ImGuiKey_V, false) && selected && clipable) {
			pasteSelectedNodes();

		} else if (ImGui::IsKeyPressed(ImGuiKey_D, false) && selected) {
			duplicateSelectedNodes();
		}
	}
}


//
//	OtGraphEditor::renderEditor
//

void OtGraphEditor::renderEditor() {
	// evaluate the graph
	auto graph = asset->getGraph();
	graph->evaluate();

	// render the graph
	widget->render(graph);

	// handle node editing
	uint32_t node;

	if (widget->isNodeEdited(node)) {
		nextTask = std::make_shared<OtEditNodeTask>(graph, node);
	}

	// handle link creations
	uint32_t from;
	uint32_t to;

	if (widget->isCreatingLink(from, to)) {
		nextTask = std::make_shared<OtCreateLinkTask>(graph, from, to);
	}

	// handle link deletion
	if (widget->isDroppingLink(from, to)) {
		nextTask = std::make_shared<OtDeleteLinkTask>(graph, from, to);
	}

	// handle link redirection
	uint32_t newTo;

	if (widget->isChangingLink(from, to, newTo)) {
		nextTask = std::make_shared<OtChangeLinkTask>(graph, from, to, newTo);
	}

	// handle node dragging
	ImVec2 offset;

	if (widget->isDraggingComplete(offset)) {
		nextTask = std::make_shared<OtDragNodesTask>(graph, offset);
	}

	// handle context menu
	if (widget->showContextMenu()) {
		ImGui::OpenPopup("Background Context");
	}

	if (ImGui::BeginPopup("Background Context")) {
		auto nodePosition = widget->screenToWidget(ImGui::GetMousePosOnOpeningCurrentPopup());
		ImGui::TextUnformatted("Create Node");
		ImGui::Separator();

		auto factory = asset->getGraph()->getNodeFactory();

		factory.eachCategory([&](OtGraphNodeCategory& category) {
			if (ImGui::BeginMenu(category.name.c_str())) {
				category.eachType([&](OtGraphNodeType& type) {
					if (ImGui::MenuItem(type.name.c_str())) {
						nextTask = std::make_shared<OtCreateNodeTask>(
							graph,
							type.name,
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
