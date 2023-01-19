//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>
#include <functional>

#include "imgui.h"
#include "ImGuizmo.h"

#include "OtUi.h"

#include "OtSceneEditor.h"
#include "OtScriptRunner.h"
#include "OtWorkspace.h"


//
//	OtSceneEditorClass::load
//

void OtSceneEditorClass::load() {
}


//
//	OtSceneEditorClass::save
//

void OtSceneEditorClass::save() {
}


//
//	OtSceneEditorClass::render
//

void OtSceneEditorClass::render() {
	// create the window
	ImGui::BeginChild((id).c_str(), ImVec2(0.0, 0.0), true, ImGuiWindowFlags_MenuBar);
	determinePanelSizes();

	// render the enditor parts
	renderMenu();
	renderPanels();
	OtUiSplitterHorizontal(&panelWidth, minPanelWidth, maxPanelWidth);
	renderScene();

	ImGui::EndChild();

	// perform any hierarchy removals
	if (scene->isValidEntity(entityToBeRemoved)) {
		if (entityToBeRemoved == selectedEntity) {
			selectedEntity = OtNullEntity;
		}

		scene->removeEntity(entityToBeRemoved);
		entityToBeRemoved = OtNullEntity;
	}

	// perform any hierarchy moves
	if (scene->isValidEntity(entityToBeMovedBefore)) {
		scene->moveEntityBefore(entityToBeMovedBefore, entityToBeMoved);
		entityToBeMovedBefore = OtNullEntity;
		entityToBeMoved = OtNullEntity;
	}

	if (scene->isValidEntity(entityToBeMovedInto)) {
		scene->moveEntityTo(entityToBeMovedInto, entityToBeMoved);
		entityToBeMovedInto = OtNullEntity;
		entityToBeMoved = OtNullEntity;
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
	auto isShortcut = (isOSX ? (super && !ctrl) : (ctrl && !super)) && !alt && !shift;

	// get status
	bool runnable = !OtScriptRunnerClass::instance()->isRunning() && !isDirty() && fileExists();

	// handle keyboard shortcuts
	if (isShortcut) {
		if (ImGui::IsKeyPressed(ImGuiKey_G)) {
			guizmoVisible = !guizmoVisible;

		} else if (ImGui::IsKeyPressed(ImGuiKey_T)) {
			guizmoOperation = ImGuizmo::TRANSLATE;

		} else if (ImGui::IsKeyPressed(ImGuiKey_R)) {
			guizmoOperation = ImGuizmo::ROTATE;

		} else if (ImGui::IsKeyPressed(ImGuiKey_S)) {
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
			if (ImGui::MenuItem("Undo", SHORTCUT "Z", nullptr, isDirty())) { }
#if __APPLE__
			if (ImGui::MenuItem("Redo", "^" SHORTCUT "Z", nullptr, false)) { }
#else
			if (ImGui::MenuItem("Redo", SHORTCUT "Y", nullptr, false)) { }
#endif

			ImGui::Separator();
			if (ImGui::MenuItem("Copy", SHORTCUT "C", nullptr, false)) { }
			if (ImGui::MenuItem("Cut", SHORTCUT "X", nullptr, false)) { }
			if (ImGui::MenuItem("Paste", SHORTCUT "V", nullptr, false)) { }
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
				OtUiDragFloat("##Snap", glm::value_ptr(snap), 3, 0.0, 0.0, 0.1, "%.2f");
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
	ImGui::BeginChild((id + "panels").c_str(), ImVec2(panelWidth, 0.0));
	lineHeight = ImGui::GetFrameHeight();
	spaceAvailable = ImGui::GetContentRegionAvail().x;

	// create the entities panel
	ImGui::BeginChild((id + "entities").c_str(), ImVec2(0.0, entityPanelHeight), true);
	renderEntitiesPanel();
	ImGui::EndChild();

	// render splitter between entity and component panels
	OtUiSplitterVertical(&entityPanelHeight, minEntityPanelHeight, maxEntityPanelHeight);

	// create the components panel
	ImGui::BeginChild((id + "components").c_str(), ImVec2(0.0, 0.0), true);
	renderComponentsPanel();
	ImGui::EndChild();

	// close the panels
	ImGui::EndChild();
}


//
//	OtSceneEditorClass::renderEntitiesPanel
//

void OtSceneEditorClass::renderEntitiesPanel() {
	spaceAvailable = ImGui::GetContentRegionAvail().x;

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
	spaceAvailable = ImGui::GetContentRegionAvail().x;

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

				if (ImGui::InputText("Entity name", buffer, sizeof(buffer) - 1)) {
					nameComponent.name = std::string(buffer);
				}

				// render entity ID
				auto id = std::to_string(scene->getComponent<OtIdComponent>(selectedEntity).id);
				ImGui::InputText("Entity ID", (char*) id.c_str(), id.size(), ImGuiInputTextFlags_ReadOnly);

				// render component editors
				renderComponent<OtTransformComponent>("Transform");
				renderComponent<OtGeometryComponent>("Geometry");
		}
	});
}


//
//	OtSceneEditorClass::renderScene
//

void OtSceneEditorClass::renderScene() {
	// create the window
	ImGui::BeginChild((id + "scene").c_str(), ImVec2(0.0, 0.0), true);
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

void OtSceneEditorClass::renderPanel(const std::string &name, bool canAdd, std::function<void()> menu, std::function<void()> content) {
	// create a tree node for the entities list
	ImGuiTreeNodeFlags flags =
		ImGuiTreeNodeFlags_DefaultOpen |
		ImGuiTreeNodeFlags_AllowItemOverlap |
		ImGuiTreeNodeFlags_Framed;

	bool open = ImGui::TreeNodeEx(name.c_str(), flags);

	// add a button to add more things (if required)
	if (canAdd) {
		ImGui::SameLine(spaceAvailable - lineHeight * 0.5);
		auto popupName = name + "Menu";

		if (ImGui::Button("+", ImVec2(lineHeight, lineHeight))) {
			ImGui::OpenPopup((popupName.c_str()));
		}

		// create menu to add things
		if (ImGui::BeginPopup(popupName.c_str())) {
			menu();
			ImGui::EndPopup();
		}
	}

	// render things if tree node is open
	if (open) {
		content();
		ImGui::TreePop();
	}
}

//
//	OtSceneEditorClass::renderEntity
//

void OtSceneEditorClass::renderEntity(OtEntity entity) {
	// get information on the entity
	auto id = std::to_string(scene->getComponent<OtIdComponent>(entity).id);
	auto& name = scene->getComponent<OtNameComponent>(entity).name;

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
	bool open = ImGui::TreeNodeEx(id.c_str(), flags, "%s", name.c_str());

	// entities are drag sources
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		ImGui::SetDragDropPayload("entity", &entity, sizeof(entity));
		ImGui::Text("%s", name.c_str());
		ImGui::EndDragDropSource();
	}

	// entities are drag targets
	OtEntity dragSourceEntity = OtNullEntity;
	ImGui::PushStyleColor(ImGuiCol_DragDropTarget, 0x8000b0b0);

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("entity")) {
			entityToBeMovedInto = entity;
			std::memcpy(&entityToBeMoved, payload->Data, sizeof(OtEntity));
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::PopStyleColor();

	// select/deselect current entity
	if (ImGui::IsItemClicked()) {
		if (selectedEntity == entity) {
			selectedEntity = OtNullEntity;

		} else {
			selectedEntity = entity;
		}
	}

	// add button to remove an entity
	ImGui::SameLine(spaceAvailable - lineHeight * 1.5 - ImGui::GetStyle().FramePadding.y);

	if (ImGui::Button(("x##" + id + "remove").c_str(), ImVec2(lineHeight, lineHeight))) {
		entityToBeRemoved = entity;
	}

	// add button to add an entity
	ImGui::SameLine(spaceAvailable - lineHeight * 0.5);
	auto popupName = id + "addmenu";

	if (ImGui::Button(("+##" + id + "add").c_str(), ImVec2(lineHeight, lineHeight))) {
		ImGui::OpenPopup((popupName.c_str()));
	}

	if (ImGui::BeginPopup(popupName.c_str())) {
		renderNewEntitiesMenu(entity);
		ImGui::EndPopup();
	}

	// render children (if required)
	if (open) {
		renderChildEntities(entity);
		ImGui::TreePop();
	}
}


//
//	OtSceneEditorClass::renderChildEntities
//

void OtSceneEditorClass::renderChildEntities(OtEntity entity) {
	// spacing between child is provided by invisible drop targets
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	// determine height of drop targets
	auto height = ImGui::GetStyle().FramePadding.y;

	// process all children
	auto child = scene->getFirstChild(entity);

	while (scene->isValidEntity(child)) {
		// create drop target
		ImGui::InvisibleButton(("##before" + std::to_string((uint32_t) child)).c_str(), ImVec2(spaceAvailable, height));
		ImGui::PushStyleColor(ImGuiCol_DragDropTarget, 0x8000b0b0);

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("entity")) {
				entityToBeMovedBefore = child;
				std::memcpy(&entityToBeMoved, payload->Data, sizeof(OtEntity));
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::PopStyleColor();

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
		selectedEntity = scene->createEntity("untitiled", entity);
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
			ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_AllowItemOverlap;

		bool open = ImGui::TreeNodeEx("##header", flags, "%s", name.c_str());

		// add button to remove the component
		ImGui::SameLine(spaceAvailable - lineHeight * 0.5);
		bool remove = ImGui::Button(("x##" + id + "remove").c_str(), ImVec2(lineHeight, lineHeight));

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
	return false;
}


//
//	OtSceneEditorClass::create
//

OtSceneEditor OtSceneEditorClass::create(const std::string& filename) {
	OtSceneEditor editor = std::make_shared<OtSceneEditorClass>();
	editor->setFileName(filename);

	if (editor->fileExists()) {
		editor->load();

	} else {
		editor->scene = OtScene2Class::create();
	}

	return editor;
}
