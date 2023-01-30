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
#include "imgui_internal.h"
#include "ImGuizmo.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtSceneEditor.h"
#include "OtTaskManager.h"
#include "OtWorkspace.h"

#include "OtCreateEntityTask.h"
#include "OtDeleteEntityTask.h"
#include "OtMoveEntityTask.h"

#include "OtCreateComponentTask.h"
#include "OtDeleteComponentTask.h"
#include "OtEditComponentTask.h"

#include "OtCutEntityTask.h"
#include "OtCopyEntityTask.h"
#include "OtPasteEntityTask.h"
#include "OtDuplicateEntityTask.h"


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
	auto basedir = path.parent_path();
	scene->deserialize(buffer.str(), &basedir);
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

		auto basedir = path.parent_path();
		stream << scene->serialize(1, '\t', &basedir);
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
	ImGui::BeginChild("script", ImVec2(), true, ImGuiWindowFlags_MenuBar);
	determinePanelSizes();

	// determine button size
	buttonSize = ImGui::GetFrameHeight();

	// render the editor parts
	renderMenu();
	renderPanels();
	OtUiSplitterHorizontal(&panelWidth, minPanelWidth, maxPanelWidth);
	renderViewPort();

	ImGui::EndChild();

	// handle keyboard
	handleShortcuts();

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
	// get status
	bool selected = !OtEntityIsNull(selectedEntity);
	bool clipable = clipboard.size() > 0;

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
				OtUiEditVec3("##snap", snap, 0.0f, 0.0f, 0.1f, "%.2f");
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Program")) {
			if (ImGui::MenuItem("Run", SHORTCUT "R", nullptr, !isDirty() && fileExists())) { run(); }
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
	ImGui::BeginChild("panels", ImVec2(panelWidth, 0.0f));

	// create the entities panel
	auto spacing = ImGui::GetStyle().ItemSpacing;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	ImGui::BeginChild("entities", ImVec2(0.0, entityPanelHeight), true);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing);
	renderEntitiesPanel();
	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::PopStyleVar();

	// render splitter between entity and component panels
	OtUiSplitterVertical(&entityPanelHeight, minEntityPanelHeight, maxEntityPanelHeight);

	// create the components panel
	ImGui::BeginChild("components", ImVec2(), true);
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
	// render selected entity info and let user change its properties
	if (scene->isValidEntity(selectedEntity)) {
		OtUiHeader("Selected Entity", ImGui::GetWindowContentRegionMax().x - ImGui::GetCursorPos().x);
		renderComponentEditor<OtCoreComponent>();
	}

	// render the panel
	renderPanel(
		"Components",
		scene->isValidEntity(selectedEntity),
		[this]() {
			// create menu to add components
			renderNewComponents<OtSceneComponents>();
		},
		[this]() {
			// render component editors if we have a selected entity
			if (scene->isValidEntity(selectedEntity)) {
				renderComponents<OtSceneComponents>();
		}
	});
}


//
//	OtSceneEditorClass::renderViewPort
//

void OtSceneEditorClass::renderViewPort() {
	// create camera (if required)
	if (!editorCamera) {
		editorCamera = OtOrbitalCameraClass::create();
	}

	// create the window
	ImGui::BeginChild("viewport", ImVec2(), true);
	auto size = ImGui::GetContentRegionAvail();

	// update the camera and render the scene
	editorCamera->setAspectRatio(size.x / size.y);
	auto textureIndex = renderer.render(scene, editorCamera, size.x, size.y);

	// show it on the screen
	ImGui::Image((void*)(intptr_t) textureIndex, size);

	// handle mouse and keyboard interactions
	if (ImGui::IsItemHovered() && ImGui::IsKeyDown(ImGuiMod_Alt)) {
		editorCamera->handleMouseKeyboard();
	}

	ImGui::EndChild();
}


//
//	OtSceneEditorClass::determinePanelSizes
//

void OtSceneEditorClass::determinePanelSizes() {
	// get available space in window
	auto available = ImGui::GetContentRegionAvail();

	// determine panel width
	minPanelWidth = available.x * 0.05f;
	maxPanelWidth = available.x * 0.9f;

	if (panelWidth < 0.0) {
		panelWidth =  available.x * 0.25f;

	} else {
		panelWidth = std::clamp(panelWidth, minPanelWidth, maxPanelWidth);
	}

	// determine entity panel height
	minEntityPanelHeight = available.y * 0.05f;
	maxEntityPanelHeight = available.y * 0.9f;

	if (entityPanelHeight < 0.0f) {
		entityPanelHeight =  available.y * 0.3f;

	} else {
		entityPanelHeight = std::clamp(entityPanelHeight, minEntityPanelHeight, maxEntityPanelHeight);
	}
}


//
//	OtSceneEditorClass::renderPanel
//

void OtSceneEditorClass::renderPanel(const std::string& name, bool canAdd, std::function<void()> menu, std::function<void()> content) {
	// render a header
	auto padding = ImGui::GetStyle().FramePadding;
	float buttonSpace = canAdd ? buttonSize + padding.x : 0.0f;
	OtUiHeader(name.c_str(), ImGui::GetWindowContentRegionMax().x - ImGui::GetCursorPos().x - buttonSpace + 1.0f);

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
	ImGui::BeginChild("content");
	content();
	ImGui::EndChild();
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
	ImGuiTreeNodeFlags_SpanAvailWidth |
	ImGuiTreeNodeFlags_AllowItemOverlap;

	if (entity == selectedEntity) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	if (!scene->hasChildren(entity)) {
		flags |= ImGuiTreeNodeFlags_Bullet;
	}

	// create a tree node
	ImGui::PushID(createID(entity, 1));
	auto tag = scene->getEntityTag(entity);
	bool open = ImGui::TreeNodeEx("node", flags, "%s", tag.c_str());

	// is this entity selected
	if (entity == selectedEntity) {
		ImGui::GetWindowDrawList()->AddRect(
			ImGui::GetItemRectMin(),
			ImGui::GetItemRectMax(),
			0x8000FF00);
	}

	// entities are drag sources
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		ImGui::SetDragDropPayload("entity", &entity, sizeof(entity));
		ImGui::Text("%s", tag.c_str());
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
	if ((ImGui::IsItemClicked() || ImGui::IsItemClicked(1)) && !ImGui::IsItemToggledOpen()) {
		selectedEntity = ImGui::GetIO().KeyShift ? OtEntityNull : entity;
	}

	if (ImGui::BeginPopupContextWindow()) {
		if (ImGui::MenuItem("Cut")) { cutEntity(); }
		if (ImGui::MenuItem("Copy")) { copyEntity(); }
		if (ImGui::MenuItem("Paste")) { pasteEntity(); }
		if (ImGui::MenuItem("Duplicate")) { duplicateEntity(); }
		ImGui::EndPopup();
	}


	// add button to remove an entity
	auto right = ImGui::GetWindowContentRegionMax().x;
	ImGui::SameLine(right - buttonSize * 2.0f - ImGui::GetStyle().FramePadding.y);

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
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2());

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
//	OtSceneEditorClass::renderNewComponent
//

template <typename T>
void OtSceneEditorClass::renderNewComponent() {
	if (ImGui::MenuItem(T::name, nullptr, false, !scene->hasComponent<T>(selectedEntity))) {
		nextTask = std::make_shared<OtCreateComponentTask<T>>(scene, selectedEntity);
	}
}


//
//	OtSceneEditorClass::renderComponentEditor
//

template <typename T>
void OtSceneEditorClass::renderComponentEditor() {
	auto& component = scene->getComponent<T>(selectedEntity);
	auto oldValue = component.serialize(nullptr).dump();

	if (component.renderGUI()) {
		auto newValue = component.serialize(nullptr).dump();
		nextTask = std::make_shared<OtEditComponentTask<T>>(scene, selectedEntity, oldValue, newValue);
	}
}


//
//	OtSceneEditorClass::renderComponent
//

template <typename T>
void OtSceneEditorClass::renderComponent(bool canRemove) {
	// only render if entity has this component
	if (scene->hasComponent<T>(selectedEntity)) {
		// add a new ID to the stack to avoid collisions
		ImGui::PushID(reinterpret_cast<void*>(typeid(T).hash_code()));

		// create a tree node for the component
		ImGuiTreeNodeFlags flags =
			ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_AllowItemOverlap;

		bool open = ImGui::TreeNodeEx("##header", flags, T::name);
		bool removeComponent = false;

		// add button to remove the component
		if (canRemove) {
			ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - buttonSize);
			removeComponent = ImGui::Button("x##remove", ImVec2(buttonSize, buttonSize));
		}

		// render the component editor (if required)
		if (open) {
			renderComponentEditor<T>();
			ImGui::TreePop();
		}

		// render a seperator line
		ImGui::Separator();

		// remove component (if required)
		if (removeComponent) {
			nextTask = std::make_shared<OtDeleteComponentTask<T>>(scene, selectedEntity);
		}

		// clean ID stack
		ImGui::PopID();
	}
}


//
//	OtSceneEditorClass::handleShortcuts
//

void OtSceneEditorClass::handleShortcuts() {
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

	// handle keyboard shortcuts
	if (isShortcut) {
		if (shift && ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
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
