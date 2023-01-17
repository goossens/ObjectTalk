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

#include "glm/ext.hpp"
#include "imgui.h"
#include "ImGuizmo.h"

#include "OtIdeUi.h"
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
	// determine panel width
	auto displaySize = ImGui::GetIO().DisplaySize.x;
	auto minPanelWidth = displaySize * 0.05f;
	auto maxPanelWidth = displaySize * 0.90f;

	if (panelWidth < 0.0) {
		panelWidth =  displaySize * 0.3;

	} else {
		panelWidth = std::clamp(panelWidth, minPanelWidth, maxPanelWidth);
	}

	// create the window
	ImGui::BeginChild((id).c_str(), ImVec2(0.0, 0.0), true, ImGuiWindowFlags_MenuBar);

	// render the menu
	renderMenu();

	// render splitter
	OtIdeUiSplitterHorizontal(&panelWidth, minPanelWidth, maxPanelWidth);

	// render the panels
	renderPanels();

	// render the scene
	ImGui::SameLine();
	renderScene();

	ImGui::EndChild();
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
				OtIdeUiDragFloat("##Snap", glm::value_ptr(snap), 3, 0.0, 0.0, 0.1, "%.2f");
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
	ImGui::BeginChild((id + "panels").c_str(), ImVec2(panelWidth, 0.0), true);

	// determine available space and line height
	spaceAvailable = ImGui::GetContentRegionAvail().x;
	lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0;

	// render our panels
	renderEntitiesPanel();
	renderComponentsPanel();

	// close window
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
			if (ImGui::MenuItem("Empty Entity")) {
				selectedEntity = scene->createEntity();
			}
		},
		[this]() {
			scene->each([&](OtEntity entity) {
				// get entity ID and name
				auto id = std::to_string(entity.getComponent<OtIdComponent>().id);
				auto& name = entity.getComponent<OtNameComponent>().name;

				// create a tree node with a bullet to act as a leave
				ImGuiTreeNodeFlags flags =
					ImGuiTreeNodeFlags_Bullet |
					ImGuiTreeNodeFlags_FramePadding |
					ImGuiTreeNodeFlags_AllowItemOverlap;

				if (entity == selectedEntity) {
					flags |= ImGuiTreeNodeFlags_Selected;
				}

				bool open = ImGui::TreeNodeEx(id.c_str(), flags, "%s", name.c_str());

				// select/deselect current entity
				if (ImGui::IsItemClicked()) {
					if (selectedEntity == entity) {
						selectedEntity = {};

					} else {
						selectedEntity = entity;
					}
				}

				// add button to remove an entity
				ImGui::SameLine(spaceAvailable - lineHeight * 0.5);

				if (ImGui::Button(("x##" + id + "remove").c_str(), ImVec2(lineHeight, lineHeight))) {
					scene->removeEntity(entity);

					if (entity == selectedEntity) {
						selectedEntity = OtEntity();
					}
				}

				// do some cleanup (if required)
				if (open) {
					ImGui::TreePop();
				}
			});
		});
}


//
//	OtSceneEditorClass::renderComponentsPanel
//

void OtSceneEditorClass::renderComponentsPanel() {
	renderPanel(
		"Components",
		selectedEntity,
		[this]() {
			// create menu to add components
			if (ImGui::MenuItem("Transform", nullptr, false, !selectedEntity.hasComponent<OtTransformComponent>())) {
				selectedEntity.addComponent<OtTransformComponent>();
			}

			if (ImGui::MenuItem("Geometry", nullptr, false, !selectedEntity.hasComponent<OtGeometryComponent>())) {
				selectedEntity.addComponent<OtGeometryComponent>();
			}
		},
		[this]() {
			// only work on the selected entity
			if (selectedEntity) {
				auto id = std::to_string(selectedEntity.getComponent<OtIdComponent>().id);
				ImGui::InputText("Unique ID", (char*) id.c_str(), id.size(), ImGuiInputTextFlags_ReadOnly);

				char buffer[256];
				auto& nameComponent = selectedEntity.getComponent<OtNameComponent>();
				std::strncpy(buffer, nameComponent.name.c_str(), sizeof(buffer) - 1);

				if (ImGui::InputText("Name", buffer, sizeof(buffer) - 1)) {
					nameComponent.name = std::string(buffer);
				}

				// render component editors
				renderComponent<OtTransformComponent>("Transform", [this] (OtTransformComponent& transform) {
					// render transformation matrix
					OtIdeUiDragFloat("Translate", glm::value_ptr(transform.translation), 3, 0.1, 0.0, 0.0, "%.2f");
					glm::vec3 rotation = glm::degrees(transform.rotation);
					OtIdeUiDragFloat("Rotate", glm::value_ptr(rotation), 3, 0.1, 0.0, 0.0, "%.2f");
					transform.rotation = glm::radians(rotation);
					OtIdeUiDragFloat("Scale", glm::value_ptr(transform.scale), 3, 0.1, 0.0, 0.0, "%.2f");
				});

				renderComponent<OtGeometryComponent>("Geometry", [] (OtGeometryComponent& geometry) {
			});
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
//	OtSceneEditorClass::renderPanel
//

void OtSceneEditorClass::renderPanel(const std::string& name, bool canAdd, std::function<void()> menu, std::function<void()> content) {
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
//	OtSceneEditorClass::renderComponent
//

template<typename T, typename R>
void OtSceneEditorClass::renderComponent(const std::string& name, R render) {
	// only render if entity has this component
	if (selectedEntity.hasComponent<T>()) {
		// add a new ID to the stack to avoid collisions
		ImGui::PushID(reinterpret_cast<void*>(typeid(T).hash_code()));

		// create a tree node for the component
		ImGuiTreeNodeFlags flags =
			ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_AllowItemOverlap;

		bool open = ImGui::TreeNodeEx("##header", flags, "%s", name.c_str());

		// add button to remove component
		ImGui::SameLine(spaceAvailable - lineHeight * 0.5);
		bool remove = ImGui::Button(("x##" + id + "remove").c_str(), ImVec2(lineHeight, lineHeight));

		// render the component editor (if required)
		if (open) {
			render(selectedEntity.getComponent<T>());
			ImGui::TreePop();
		}

		// remove entity if required
		if (remove) {
			selectedEntity.removeComponent<T>();
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
