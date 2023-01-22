//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>
#include <fstream>
#include <functional>
#include <sstream>

#include "glm/ext.hpp"
#include "imgui.h"
#include "ImGuizmo.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtSceneEditor.h"
#include "OtTaskManager.h"
#include "OtWorkspace.h"

#include "OtCreateEntityTask.h"
#include "OtDeleteEntityTask.h"
#include "OtDuplicateEntityTask.h"
#include "OtMoveEntityTask.h"
#include "OtCutEntityTask.h"
#include "OtCopyEntityTask.h"
#include "OtPasteEntityTask.h"


//
//	createID
//

static inline void* createID(OtEntity entity, uint32_t key) {
	return (void*) ((((uint64_t) key) << 32) | (uint64_t) entity);
}


//
//	OtSceneEditorClass::load
//

void OtSceneEditorClass::load() {
	// load scene from file
	std::stringstream buffer;

	try {
		std::ifstream stream(path.c_str());

		if (stream.fail()) {
			OtExcept("Can't read from file [%s]", path.c_str());
		}

		buffer << stream.rdbuf();
		stream.close();

	} catch (std::exception& e) {
		OtExcept("Can't read from file [%s], error: %s", path.c_str(), e.what());
	}

	// recreate the scene
	scene->deserialize(buffer.str());
}


//
//	OtSceneEditorClass::save
//

void OtSceneEditorClass::save() {
	try {
		// write scene to file
		std::ofstream stream(path.c_str());

		if (stream.fail()) {
			OtExcept("Can't write to file [%s]", path.c_str());
		}

		stream << scene->serialize(1, '\t');
		stream.close();

	} catch (std::exception& e) {
		OtExcept("Can't write to file [%s], error: %s", path.c_str(), e.what());
	}

	// reset current version number (marking the content as clean)
	version = taskManager.getUndoCount();
}


//
//	OtSceneEditorClass::render
//

void OtSceneEditorClass::render() {
	// create the window
	ImGui::BeginChild("script", ImVec2(0.0, 0.0), true, ImGuiWindowFlags_MenuBar);
	determinePanelSizes();

	// determine button size
	buttonSize = ImGui::GetFrameHeight();

	// render the enditor parts
	renderMenu();
	renderPanels();
	OtUiSplitterHorizontal(&panelWidth, minPanelWidth, maxPanelWidth);
	renderViewPort();

	ImGui::EndChild();

	// perform editing task (if required)
	if (nextTask) {
		taskManager.perform(nextTask);
		nextTask = nullptr;

		// unset selected entity if it is no longer valid
		if (!scene->isValidEntity(selectedEntity)) {
			selectedEntity = OtEntityNull;
		}
	}
}


//
//	OtSceneEditorClass::renderMenu
//

void OtSceneEditorClass::renderMenu() {
	// get keyboard state to handle keyboard shortcuts
	ImGuiIO& io = ImGui::GetIO();
	auto isOSX = io.ConfigMacOSXBehaviors;
	auto alt = io.KeyAlt;
	auto ctrl = io.KeyCtrl;
	auto shift = io.KeyShift;
	auto super = io.KeySuper;
	auto isShortcut = isOSX ? super : ctrl;

	// get status
	bool selected = !OtEntityIsNull(selectedEntity);
	bool clipable = clipboard.size() > 0;
	bool runnable = !isDirty() && fileExists();

	// handle keyboard shortcuts
	if (isShortcut) {
		if (shift && ImGui::IsKeyPressed(ImGuiKey_Z, false) ) {
			if (taskManager.canRedo()) {
				taskManager.redo();
			}

		} else if (ImGui::IsKeyPressed(ImGuiKey_Z, false) && taskManager.canUndo()) {
			taskManager.undo();

		} else if (ImGui::IsKeyPressed(ImGuiKey_X, false) && selected) {
			cutEntity();

		} else if (ImGui::IsKeyPressed(ImGuiKey_C, false) && selected) {
			copyEntity();

		} else if (ImGui::IsKeyPressed(ImGuiKey_V, false) && selected && clipable) {
			pasteEntity();

		} else if (ImGui::IsKeyPressed(ImGuiKey_D, false) && selected) {
			duplicateEntity();

		} else if (ImGui::IsKeyPressed(ImGuiKey_G, false)) {
			guizmoVisible = !guizmoVisible;

		} else if (ImGui::IsKeyPressed(ImGuiKey_T, false)) {
			guizmoOperation = ImGuizmo::TRANSLATE;

		} else if (ImGui::IsKeyPressed(ImGuiKey_R, false)) {
			guizmoOperation = ImGuizmo::ROTATE;

		} else if (ImGui::IsKeyPressed(ImGuiKey_S, false)) {
			guizmoOperation = ImGuizmo::SCALE;
		}
	}

#if __APPLE__
#define SHORTCUT "Cmd-"
#else
#define SHORTCUT "Ctrl-"
#endif

	// create menubar
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {  // shortcuts handled by workspace
			if (ImGui::MenuItem("New", SHORTCUT "N")) { OtWorkspaceClass::instance()->newFile(); }
			if (ImGui::MenuItem("Open...", SHORTCUT "O")) { OtWorkspaceClass::instance()->openFile(); }
			ImGui::Separator();

			if (fileExists()) {
				if (ImGui::MenuItem("Save", SHORTCUT "S", nullptr, isDirty())) { OtWorkspaceClass::instance()->saveFile(); }
				if (ImGui::MenuItem("Save As...")) { OtWorkspaceClass::instance()->saveAsFile(); }

			} else {
				if (ImGui::MenuItem("Save As...", SHORTCUT "S", nullptr, isDirty())) { OtWorkspaceClass::instance()->saveAsFile(); }
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Close", SHORTCUT "W")) { OtWorkspaceClass::instance()->closeFile(); }

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", SHORTCUT "Z", nullptr, taskManager.canUndo())) { taskManager.undo(); }
#if __APPLE__
			if (ImGui::MenuItem("Redo", "^" SHORTCUT "Z", nullptr, taskManager.canRedo())) { taskManager.redo(); }
#else
			if (ImGui::MenuItem("Redo", SHORTCUT "Y", nullptr, taskManager.canRedo())) { taskManager.redo(); }
#endif

			ImGui::Separator();
			if (ImGui::MenuItem("Cut", SHORTCUT "X", nullptr, selected)) { cutEntity(); }
			if (ImGui::MenuItem("Copy", SHORTCUT "C", nullptr, selected)) { copyEntity(); }
			if (ImGui::MenuItem("Paste", SHORTCUT "V", nullptr, selected && clipable)) { pasteEntity(); }
			if (ImGui::MenuItem("Duplicate", SHORTCUT "D", nullptr, selected)) { duplicateEntity(); }
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) {
			ImGui::MenuItem("Gizmo", SHORTCUT "G", &guizmoVisible);

			if (ImGui::BeginMenu("Gizmo Mode", guizmoVisible)) {
				if (ImGui::RadioButton("Translate", guizmoOperation == ImGuizmo::TRANSLATE)) {
					guizmoOperation = ImGuizmo::TRANSLATE;
					guizmoVisible = true;
				}

				ImGui::SameLine();

				if (ImGui::RadioButton("Rotate", guizmoOperation == ImGuizmo::ROTATE)) {
					guizmoOperation = ImGuizmo::ROTATE;
					guizmoVisible = true;
				}

				ImGui::SameLine();

				if (ImGui::RadioButton("Scale", guizmoOperation == ImGuizmo::SCALE)) {
					guizmoOperation = ImGuizmo::SCALE;
					guizmoVisible = true;
				}

				ImGui::EndMenu();
			}

			// render snap control
			if (ImGui::BeginMenu("Gizmo Snap", guizmoVisible)) {
				ImGui::Checkbox("Snaping", &guizmoSnapping);
				OtUiDragFloat("##snap", glm::value_ptr(snap), 3, 0.0, 0.0, 0.1, "%.2f");
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Program")) {
			if (ImGui::MenuItem("Run", SHORTCUT "R", nullptr, runnable)) { run(); }
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

//
//	OtSceneEditorClass::renderPanels
//

void OtSceneEditorClass::renderPanels() {
	// create a new child window
	ImGui::BeginChild("panels", ImVec2(panelWidth, 0.0));

	// create the entities panel
	ImGui::BeginChild("entities", ImVec2(0.0, entityPanelHeight), true);
	renderEntitiesPanel();
	ImGui::EndChild();

	// render splitter between entity and component panels
	OtUiSplitterVertical(&entityPanelHeight, minEntityPanelHeight, maxEntityPanelHeight);

	// create the components panel
	ImGui::BeginChild("components", ImVec2(0.0, 0.0), true);
	renderComponentsPanel();
	ImGui::EndChild();

	// close the panels
	ImGui::EndChild();
}


//
//	OtSceneEditorClass::renderEntitiesPanel
//

void OtSceneEditorClass::renderEntitiesPanel() {
	renderPanel(
		"Entities",
		true,
		[this]() {
			// create menu to add entities
			renderNewEntitiesMenu(scene->getRootEntity());
		},
		[this]() {
			// render all root entities
			renderChildEntities(scene->getRootEntity());
		});
}


//
//	OtSceneEditorClass::renderComponentsPanel
//

void OtSceneEditorClass::renderComponentsPanel() {
	renderPanel(
		"Components",
		scene->isValidEntity(selectedEntity),
		[this]() {
			// create menu to add components
			if (ImGui::MenuItem("Transform", nullptr, false, !scene->hasComponent<OtTransformComponent>(selectedEntity))) {
				scene->addComponent<OtTransformComponent>(selectedEntity);
			}

			if (ImGui::MenuItem("Geometry", nullptr, false, !scene->hasComponent<OtGeometryComponent>(selectedEntity))) {
				scene->addComponent<OtGeometryComponent>(selectedEntity);
			}
		},
		[this]() {
			// only work if we have a selected entity
			if (scene->isValidEntity(selectedEntity)) {
				// render entity name
				char buffer[256];
				auto& nameComponent = scene->getComponent<OtNameComponent>(selectedEntity);
				std::strncpy(buffer, nameComponent.name.c_str(), sizeof(buffer) - 1);

				if (ImGui::InputText("Entity", buffer, sizeof(buffer) - 1)) {
					nameComponent.name = std::string(buffer);
				}

				// render component editors
				renderComponent<OtTransformComponent>("Transform");
				renderComponent<OtGeometryComponent>("Geometry");
		}
	});
}


//
//	OtSceneEditorClass::renderViewPort
//

void OtSceneEditorClass::renderViewPort() {
	// create the window
	ImGui::BeginChild("viewport", ImVec2(0.0, 0.0), true);
	ImGui::EndChild();
}


//
//	OtSceneEditorClass::determinePanelSizes
//

void OtSceneEditorClass::determinePanelSizes() {
	// get available space in window
	auto available = ImGui::GetContentRegionAvail();

	// determine panel width
	minPanelWidth = available.x * 0.05;
	maxPanelWidth = available.x * 0.9;

	if (panelWidth < 0.0) {
		panelWidth =  available.x * 0.3;

	} else {
		panelWidth = std::clamp(panelWidth, minPanelWidth, maxPanelWidth);
	}

	// determine entity panel height
	minEntityPanelHeight = available.y * 0.05;
	maxEntityPanelHeight = available.y * 0.9;

	if (entityPanelHeight < 0.0) {
		entityPanelHeight =  available.y * 0.3;

	} else {
		entityPanelHeight = std::clamp(entityPanelHeight, minEntityPanelHeight, maxEntityPanelHeight);
	}
}


//
//	OtSceneEditorClass::renderPanel
//

void OtSceneEditorClass::renderPanel(const std::string& name, bool canAdd, std::function<void()> menu, std::function<void()> content) {
	// render a header
	auto windowPos = ImGui::GetWindowPos();
	auto cursorPos = ImGui::GetCursorPos();
	auto padding = ImGui::GetStyle().FramePadding;

	float buttonSpace = canAdd ? buttonSize + padding.y : 0;
	ImVec2 topLeft(windowPos.x + cursorPos.x, windowPos.y + cursorPos.y);
	ImVec2 bottomRight(windowPos.x + ImGui::GetWindowContentRegionMax().x - buttonSpace, topLeft.y + buttonSize);

	auto color = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_TitleBg]);
	ImGui::GetWindowDrawList()->AddRectFilled(topLeft, bottomRight, color);
	ImGui::SameLine(cursorPos.x + padding.x);
	ImGui::AlignTextToFramePadding();
	ImGui::Text("%s", name.c_str());

	// add a button to add more things (if required)
	if (canAdd) {
		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - buttonSize);
		auto popupName = name + "Menu";

		if (ImGui::Button("+", ImVec2(buttonSize, buttonSize))) {
			ImGui::OpenPopup((popupName.c_str()));
		}

		// create menu to add things
		if (ImGui::BeginPopup(popupName.c_str())) {
			menu();
			ImGui::EndPopup();
		}
	}

	// render panel context
	content();
}

//
//	OtSceneEditorClass::renderEntity
//

void OtSceneEditorClass::renderEntity(OtEntity entity) {
	// determine flags
	ImGuiTreeNodeFlags flags =
		ImGuiTreeNodeFlags_DefaultOpen |
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_FramePadding |
		ImGuiTreeNodeFlags_AllowItemOverlap;

	// is this a leave node?
	if (!scene->hasChildren(entity)) {
		flags |= ImGuiTreeNodeFlags_Bullet;
	}

	// is this entity selected
	if (entity == selectedEntity) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	// create a tree node
	ImGui::PushID(createID(entity, 1));
	auto& name = scene->getComponent<OtNameComponent>(entity).name;
	bool open = ImGui::TreeNodeEx("node", flags, "%s", name.c_str());

	// entities are drag sources
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		ImGui::SetDragDropPayload("entity", &entity, sizeof(entity));
		ImGui::Text("%s", name.c_str());
		ImGui::EndDragDropSource();
	}

	// entities are drag targets
	OtEntity dragSourceEntity = OtEntityNull;
	ImGui::PushStyleColor(ImGuiCol_DragDropTarget, 0x8000b0b0);

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("entity")) {
			OtEntity entityToBeMoved;
			std::memcpy(&entityToBeMoved, payload->Data, sizeof(OtEntity));
			nextTask = std::make_shared<OtMoveEntityTask>(scene, entity, entityToBeMoved, false);
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::PopStyleColor();

	// select/deselect current entity
	if (ImGui::IsItemClicked()) {
		if (selectedEntity == entity) {
			selectedEntity = OtEntityNull;

		} else {
			selectedEntity = entity;
		}
	}

	// add button to remove an entity
	auto right = ImGui::GetWindowContentRegionMax().x;
	ImGui::SameLine(right - buttonSize * 2 - ImGui::GetStyle().FramePadding.y);

	if (ImGui::Button("x##remove", ImVec2(buttonSize, buttonSize))) {
		nextTask = std::make_shared<OtDeleteEntityTask>(scene, entity);
	}

	// add button to add an entity
	ImGui::SameLine(right - buttonSize);

	if (ImGui::Button("+##add", ImVec2(buttonSize, buttonSize))) {
		ImGui::OpenPopup(("addmenu"));
	}

	if (ImGui::BeginPopup("addmenu")) {
		renderNewEntitiesMenu(entity);
		ImGui::EndPopup();
	}

	// render children (if required)
	if (open) {
		renderChildEntities(entity);
		ImGui::TreePop();
	}

	ImGui::PopID();
}


//
//	OtSceneEditorClass::renderChildEntities
//

void OtSceneEditorClass::renderChildEntities(OtEntity entity) {
	// spacing between child is provided by invisible drop targets
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	// process all children
	auto child = scene->getFirstChild(entity);

	while (scene->isValidEntity(child)) {
		// create drop target
		ImGui::PushID(createID(child, 2));
		ImGui::InvisibleButton("##", ImVec2(ImGui::GetWindowContentRegionMax().x - ImGui::GetCursorPos().x, ImGui::GetStyle().FramePadding.y));
		ImGui::PushStyleColor(ImGuiCol_DragDropTarget, 0x8000b0b0);

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("entity")) {
				OtEntity entityToBeMoved;
				std::memcpy(&entityToBeMoved, payload->Data, sizeof(OtEntity));
				nextTask = std::make_shared<OtMoveEntityTask>(scene, child, entityToBeMoved, true);
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::PopStyleColor();
		ImGui::PopID();

		// render the child entity
		renderEntity(child);
		child = scene->getNextSibling(child);
	};

	ImGui::PopStyleVar();
}


//
//	OtSceneEditorClass::renderNewEntitiesMenu
//

void OtSceneEditorClass::renderNewEntitiesMenu(OtEntity entity) {
	if (ImGui::MenuItem("Empty Entity")) {
		nextTask = std::make_shared<OtCreateEntityTask>(scene, entity, OtCreateEntityTask::empty);
	}
}


//
//	OtSceneEditorClass::renderComponent
//

template<typename T>
void OtSceneEditorClass::renderComponent(const std::string& name) {
	// only render if entity has this component
	if (scene->hasComponent<T>(selectedEntity)) {
		// add a new ID to the stack to avoid collisions
		ImGui::PushID(reinterpret_cast<void*>(typeid(T).hash_code()));

		// create a tree node for the component
		ImGuiTreeNodeFlags flags =
			ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_AllowItemOverlap;

		ImGui::Separator();
		bool open = ImGui::TreeNodeEx("##header", flags, "%s", name.c_str());

		// add button to remove the component
		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - buttonSize);
		bool remove = ImGui::Button("x##remove", ImVec2(buttonSize, buttonSize));

		// render the component editor (if required)
		if (open) {
			scene->getComponent<T>(selectedEntity).renderGUI();
			ImGui::TreePop();
		}

		// remove entity if required
		if (remove) {
			scene->removeComponent<T>(selectedEntity);
		}

		// clean ID stack
		ImGui::PopID();
	}
}


//
//	OtSceneEditorClass::run
//

void OtSceneEditorClass::run() {
}


//
//	OtSceneEditorClass::isDirty
//

bool OtSceneEditorClass::isDirty() {
	return version != taskManager.getUndoCount();
}


//
//	OtSceneEditorClass::cutEntity
//

void OtSceneEditorClass::cutEntity() {
	nextTask = std::make_shared<OtCutEntityTask>(scene, selectedEntity, clipboard);
}


//
//	OtSceneEditorClass::copyEntity
//

void OtSceneEditorClass::copyEntity() {
	nextTask = std::make_shared<OtCopyEntityTask>(scene, selectedEntity, clipboard);
}


//
//	OtSceneEditorClass::pasteEntity
//

void OtSceneEditorClass::pasteEntity() {
	nextTask = std::make_shared<OtPasteEntityTask>(scene, selectedEntity, clipboard);
}


//
//	OtSceneEditorClass::duplicateEntity
//

void OtSceneEditorClass::duplicateEntity() {
	nextTask = std::make_shared<OtDuplicateEntityTask>(scene, selectedEntity);
}


//
//	OtSceneEditorClass::create
//

OtSceneEditor OtSceneEditorClass::create(const std::filesystem::path& path) {
	OtSceneEditor editor = std::make_shared<OtSceneEditorClass>();
	editor->setFilePath(path);
	editor->scene = OtScene2Class::create();

	if (editor->fileExists()) {
		editor->load();
	}

	return editor;
}