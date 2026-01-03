//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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

#include "OtAudioEditor.h"

#include "OtAlignCircuitsTask.h"
#include "OtCopyCircuitsTask.h"
#include "OtChangeWireTask.h"
#include "OtCreateWireTask.h"
#include "OtCreateCircuitTask.h"
#include "OtCutCircuitsTask.h"
#include "OtDeleteWireTask.h"
#include "OtDeleteCircuitsTask.h"
#include "OtDuplicateCircuitsTask.h"
#include "OtDragCircuitsTask.h"
#include "OtEditCircuitTask.h"
#include "OtPasteCircuitsTask.h"


//
//	OtAudioEditor::clipboard
//

std::string OtAudioEditor::clipboard;


//
//	OtAudioEditor::clear
//

void OtAudioEditor::clear() {
	audio.clear();
}


//
//	OtAudioEditor::load
//

void OtAudioEditor::load() {
	audio.load(path);
}


//
//	OtAudioEditor::saveFile
//

void OtAudioEditor::save() {
	audio.save(path);
	taskManager.baseline();
}


//
//	OtAudioEditor::cutSelectedCircuits
//

void OtAudioEditor::cutSelectedCircuits() {
	nextTask = std::make_shared<OtCutCircuitsTask>(&audio, clipboard);
}


//
//	OtAudioEditor::copySelectedCircuits
//

void OtAudioEditor::copySelectedCircuits() {
	nextTask = std::make_shared<OtCopyCircuitsTask>(&audio, clipboard);
}


//
//	OtAudioEditor::pasteSelectedCircuits
//

void OtAudioEditor::pasteSelectedCircuits() {
	nextTask = std::make_shared<OtPasteCircuitsTask>(&audio, clipboard);
}


//
//	OtAudioEditor::deleteSelectedCircuits
//

void OtAudioEditor::deleteSelectedCircuits() {
	nextTask = std::make_shared<OtDeleteCircuitsTask>(&audio);
}


//
//	OtAudioEditor::duplicateSelectedCircuits
//

void OtAudioEditor::duplicateSelectedCircuits() {
	nextTask = std::make_shared<OtDuplicateCircuitsTask>(&audio);
}


//
//	OtAudioEditor::alignSelectedCircuits
//

void OtAudioEditor::alignSelectedCircuits(OtAlignCircuitsTask::Alignment alignment) {
	nextTask = std::make_shared<OtAlignCircuitsTask>(&audio, alignment);
}


//
//	OtAudioEditor::renderMenus
//

void OtAudioEditor::renderMenus() {
	// get status
	bool selected = audio.hasSelected();
	bool multipleSelected = audio.hasMultipleSelected();

	if (ImGui::BeginMenu("Edit")) {
		if (ImGui::MenuItem("Undo", OT_UI_SHORTCUT "Z", nullptr, taskManager.canUndo())) { taskManager.undo(); }
#if __APPLE__
		if (ImGui::MenuItem("Redo", "^" OT_UI_SHORTCUT "Z", nullptr, taskManager.canRedo())) { taskManager.redo(); }
#else
		if (ImGui::MenuItem("Redo", OT_UI_SHORTCUT "Y", nullptr, taskManager.canRedo())) { taskManager.redo(); }
#endif

		ImGui::Separator();
		if (ImGui::MenuItem("Cut", OT_UI_SHORTCUT "X", nullptr, selected)) { cutSelectedCircuits(); }
		if (ImGui::MenuItem("Copy", OT_UI_SHORTCUT "C", nullptr, selected)) { copySelectedCircuits(); }
		if (ImGui::MenuItem("Paste", OT_UI_SHORTCUT "V", nullptr, clipboard.size() > 0)) { pasteSelectedCircuits(); }
		if (ImGui::MenuItem("Delete", "Del", nullptr, selected)) { deleteSelectedCircuits(); }
		if (ImGui::MenuItem("Duplicate", OT_UI_SHORTCUT "D", nullptr, selected)) { duplicateSelectedCircuits(); }
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Selection")) {
		if (ImGui::MenuItem("Select All", OT_UI_SHORTCUT "A")) { audio.selectAll(); }
		if (ImGui::MenuItem("Clear Selection")) { audio.deselectAll(); }
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("View")) {
		bool flag;
		flag = widget.isRenderingGrid(); if (ImGui::MenuItem("Show Grid", nullptr, &flag)) { widget.setGridRendering(flag); };
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Align")) {
		if (ImGui::MenuItem("Left", nullptr, nullptr, multipleSelected)) { alignSelectedCircuits(OtAlignCircuitsTask::Alignment::left); }
		if (ImGui::MenuItem("Center", nullptr, nullptr, multipleSelected)) { alignSelectedCircuits(OtAlignCircuitsTask::Alignment::center); }
		if (ImGui::MenuItem("Right", nullptr, nullptr, multipleSelected)) { alignSelectedCircuits(OtAlignCircuitsTask::Alignment::right); }
		ImGui::Separator();
		if (ImGui::MenuItem("Top", nullptr, nullptr, multipleSelected)) { alignSelectedCircuits(OtAlignCircuitsTask::Alignment::top); }
		if (ImGui::MenuItem("Middle", nullptr, nullptr, multipleSelected)) { alignSelectedCircuits(OtAlignCircuitsTask::Alignment::middle); }
		if (ImGui::MenuItem("Bottom", nullptr, nullptr, multipleSelected)) { alignSelectedCircuits(OtAlignCircuitsTask::Alignment::bottom); }
		ImGui::EndMenu();
	}
}


//
//	OtAudioEditor::handleShortcuts
//

void OtAudioEditor::handleShortcuts() {
	// get status
	bool selected = audio.hasSelected();

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
			cutSelectedCircuits();

		} else if (ImGui::IsKeyPressed(ImGuiKey_C, false) && selected) {
			copySelectedCircuits();

		} else if (ImGui::IsKeyPressed(ImGuiKey_V, false) && clipboard.size() > 0) {
			pasteSelectedCircuits();

		} else if (ImGui::IsKeyPressed(ImGuiKey_D, false) && selected) {
			duplicateSelectedCircuits();

		} else if (ImGui::IsKeyPressed(ImGuiKey_A, false)) {
			audio.selectAll();
		}

	} else if ((ImGui::IsKeyPressed(ImGuiKey_Backspace, false) || ImGui::IsKeyPressed(ImGuiKey_Delete, false)) && selected) {
		deleteSelectedCircuits();
	}
}


//
//	OtAudioEditor::renderEditor
//

void OtAudioEditor::renderEditor() {
	// given that audio processes in real-time in a separate thread, we need to lock during editing
	audio.lock();

	// render the audio
	widget.render(&audio);

	// handle circuit editing events
	uint32_t circuit;

	if (widget.isCircuitEdited(circuit)) {
		nextTask = std::make_shared<OtEditCircuitTask>(&audio, circuit);
	}

	// handle wire creations
	uint32_t from;
	uint32_t to;

	if (widget.isCreatingWire(from, to)) {
		nextTask = std::make_shared<OtCreateWireTask>(&audio, from, to);
	}

	// handle wire deletion
	if (widget.isDroppingWire(from, to)) {
		nextTask = std::make_shared<OtDeleteWireTask>(&audio, from, to);
	}

	// handle wire redirection
	uint32_t newTo;

	if (widget.isChangingWire(from, to, newTo)) {
		nextTask = std::make_shared<OtChangeWireTask>(&audio, from, to, newTo);
	}

	// handle circuit dragging
	ImVec2 offset;

	if (widget.isDraggingComplete(offset)) {
		nextTask = std::make_shared<OtDragCircuitsTask>(&audio, offset);
	}

	// handle context menu
	if (widget.isRequestingContextMenu()) {
		ImGui::OpenPopup("Background Context");
	}

	if (ImGui::BeginPopup("Background Context")) {
		auto circuitPosition = widget.getContextMenuPos();
		ImGui::TextUnformatted("Create Circuit");
		ImGui::Separator();

		OtCircuitFactory::eachCategory([&](OtCircuitCategory& category) {
			if (ImGui::BeginMenu(category.name.c_str())) {
				category.eachType([&](const std::string& type) {
					if (ImGui::MenuItem(type.c_str())) {
						nextTask = std::make_shared<OtCreateCircuitTask>(
							&audio,
							type,
							circuitPosition.x,
							circuitPosition.y);
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

	audio.unlock();
}
