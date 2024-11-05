//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>
#include <cstring>
#include <functional>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"
#include "nlohmann/json.hpp"

#include "OtAssetManager.h"
#include "OtMessageBus.h"
#include "OtUi.h"

#include "OtSceneEditor.h"
#include "OtTaskManager.h"
#include "OtWorkspace.h"

#include "OtEditPostProcessingTask.h"
#include "OtSaveEditorCameraTask.h"

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
//	OtSceneEditor::initialize
//

OtSceneEditor::OtSceneEditor() {
	renderer = std::make_unique<OtSceneRenderer>();
}


//
//	OtSceneEditor::clear
//

void OtSceneEditor::clear() {
	scene.clear();
}


//
//	OtSceneEditor::load
//

void OtSceneEditor::load() {
	scene.load(path);
	processMetaData();
}


//
//	OtSceneEditor::save
//

void OtSceneEditor::save() {
	generateMetaData();
	scene.save(path);
	taskManager.baseline();
}


//
//	OtSceneEditor::processMetaData
//

void OtSceneEditor::processMetaData() {
	// parse the metadata
	nlohmann::json metadata = nlohmann::json::parse(scene.getMetaData().c_str());

	// process metadata and set editor properties
	if (metadata.contains("camera")) {
		editorCamera.deserialize(metadata["camera"], nullptr);
	}

	if (metadata.contains("grid")) {
		auto grid = metadata["grid"];
		gridEnabled = grid.value("enabled", false);
		gridScale = grid.value("scale", 1.0f);
	}

	if (metadata.contains("gizmo")) {
		auto gizmo = metadata["gizmo"];
		guizmoEnabled = gizmo.value("enabled", false);
		guizmoOperation = gizmo.value("mode", ImGuizmo::TRANSLATE);
		guizmoSnapping = gizmo.value("snapping", false);
		guizmoSnapInterval = gizmo.value("snapInterval", glm::vec3(0.1f));
	}
}


//
//	OtSceneEditor::generateMetaData
//

void OtSceneEditor::generateMetaData() {
	// build metadata
	auto metadata = nlohmann::json::object();
	metadata["type"] = "scene";
	metadata["camera"] = editorCamera.serialize(nullptr);

	auto grid = nlohmann::json::object();
	grid["enabled"] = gridEnabled;
	grid["scale"] = gridScale;
	metadata["grid"] = grid;

	auto gizmo = nlohmann::json::object();
	gizmo["enabled"] = guizmoEnabled;
	gizmo["mode"] = guizmoOperation;
	gizmo["snapping"] = guizmoSnapping;
	gizmo["snapInterval"] = guizmoSnapInterval;
	metadata["gizmo"] = gizmo;

	// set the metadata
	scene.setMetaData(metadata.dump());
}


//
//	OtSceneEditor::setSceneCamera
//

static void makeCameraList(OtScene* scene, OtEntity entity, OtEntity* list, int& count) {
	if (count < 9) {
		if (scene->hasComponent<OtCameraComponent>(entity) && scene->hasComponent<OtTransformComponent>(entity)) {
			list[count++] = entity;
		}

		auto child = scene->getFirstChild(entity);

		while (scene->isValidEntity(child)) {
			makeCameraList(scene, child, list, count);
			child = scene->getNextSibling(child);
		}
	}
}

void OtSceneEditor::setSceneCamera(int cameraNumber) {
	// get a list of cameras
	OtEntity list[10];
	list[0] = OtEntityNull;
	int entries = 1;
	makeCameraList(&scene, scene.getRootEntity(), list, entries);

	// see if selected camera is avalable
	if (cameraNumber < entries) {
		selectedCamera = list[cameraNumber];

	} else {
		// the default answer is the editor camera
		selectedCamera = OtEntityNull;
	}
}


//
//	OtSceneEditor::renderMenu
//

void OtSceneEditor::renderMenu(bool canRun) {
	// get status
	bool selected = scene.isValidEntity(selectedEntity);
	bool clipable = clipboard.size() > 0;

	// create menubar
	if (ImGui::BeginMenuBar()) {
		renderFileMenu(canRun);

		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", OT_UI_SHORTCUT "Z", nullptr, taskManager.canUndo())) { taskManager.undo(); }
#if __APPLE__
			if (ImGui::MenuItem("Redo", "^" OT_UI_SHORTCUT "Z", nullptr, taskManager.canRedo())) { taskManager.redo(); }
#else
			if (ImGui::MenuItem("Redo", OT_UI_SHORTCUT "Y", nullptr, taskManager.canRedo())) { taskManager.redo(); }
#endif

			ImGui::Separator();
			if (ImGui::MenuItem("Cut", OT_UI_SHORTCUT "X", nullptr, selected)) { cutEntity(); }
			if (ImGui::MenuItem("Copy", OT_UI_SHORTCUT "C", nullptr, selected)) { copyEntity(); }
			if (ImGui::MenuItem("Paste", OT_UI_SHORTCUT "V", nullptr, selected && clipable)) { pasteEntity(); }
			if (ImGui::MenuItem("Duplicate", OT_UI_SHORTCUT "D", nullptr, selected)) { duplicateEntity(); }
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) {
			if (ImGui::MenuItem("Toggle Console")) { OtMessageBus::send("toggleconsole"); }
			if (ImGui::MenuItem("Toggle Debug")) { toggleRendererDebug(); }
			ImGui::Separator();

			// render camera selector
			if (ImGui::BeginMenu("Active Camera")) {
				if (ImGui::RadioButton("Editor Camera", selectedCamera == OtEntityNull)) {
					selectedCamera = OtEntityNull;
				}

				// get a list of cameras
				OtEntity list[9];
				int entries = 0;
				makeCameraList(&scene, scene.getRootEntity(), list, entries);

				for (auto i = 0; i < entries; i++) {
					if (ImGui::RadioButton(scene.getTag(list[i]).c_str(), selectedCamera == list[i])) {
						selectedCamera = list[i];
					}
				}

				ImGui::EndMenu();
			}

			// render editor camera settings
			if (ImGui::BeginMenu("Editor Camera Settings")) {
				editorCamera.renderUI();
				ImGui::EndMenu();
			}

			// allow editor camera settings to be transferred to scene camera
			if (ImGui::BeginMenu("Save Editor Camera To")) {
				// get a list of cameras
				OtEntity list[9];
				int entries = 0;
				makeCameraList(&scene, scene.getRootEntity(), list, entries);

				for (auto i = 0; i < entries; i++) {
					if (ImGui::MenuItem(scene.getTag(list[i]).c_str())) {
						saveEditorCamera(list[i]);
					}
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();
			ImGui::MenuItem("Grid", OT_UI_SHORTCUT "D", &gridEnabled);

			if (ImGui::BeginMenu("Grid Scale", gridEnabled)) {
				OtUi::dragFloat("##scale", &gridScale, 0.1f, 100.0f);
				ImGui::EndMenu();
			}

			ImGui::Separator();
			ImGui::MenuItem("Gizmo", OT_UI_SHORTCUT "G", &guizmoEnabled);

			if (ImGui::BeginMenu("Gizmo Mode", guizmoEnabled)) {
				if (ImGui::RadioButton("Translate", guizmoOperation == ImGuizmo::TRANSLATE)) {
					guizmoOperation = ImGuizmo::TRANSLATE;
				}

				if (ImGui::RadioButton("Rotate", guizmoOperation == ImGuizmo::ROTATE)) {
					guizmoOperation = ImGuizmo::ROTATE;
				}

				if (ImGui::RadioButton("Scale", guizmoOperation == ImGuizmo::SCALE)) {
					guizmoOperation = ImGuizmo::SCALE;
				}

				ImGui::EndMenu();
			}

			// render snap control
			if (ImGui::BeginMenu("Gizmo Snap", guizmoEnabled)) {
				OtUi::toggleButton("Snaping", &guizmoSnapping);
				OtUi::editVec3("##Interval", &guizmoSnapInterval, 0.0f, 0.1f);
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		// render post processing menu
		if (ImGui::BeginMenu("Processing")) {
			auto& processor = scene.getPostProcessing();
			auto oldValue = processor.serialize(nullptr).dump();

			OtUi::header("Post Proceessing Settings");
			ImGui::Dummy(ImVec2(0.0f, 0.0f));

			if (processor.renderUI()) {
				auto newValue = processor.serialize(nullptr).dump();
				nextTask = std::make_shared<OtEditPostProcessingTask>(&scene, oldValue, newValue);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	// handle keyboard shortcuts (if required)
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
		// handle menu shortcuts
		if (ImGui::IsKeyDown(ImGuiMod_Ctrl)) {
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
				cutEntity();

			} else if (ImGui::IsKeyPressed(ImGuiKey_C, false) && selected) {
				copyEntity();

			} else if (ImGui::IsKeyPressed(ImGuiKey_V, false) && selected && clipable) {
				pasteEntity();

			} else if (ImGui::IsKeyPressed(ImGuiKey_D, false) && selected) {
				duplicateEntity();

			} else if (ImGui::IsKeyPressed(ImGuiKey_G, false)) {
				guizmoEnabled = !guizmoEnabled;
			}

		// handle camera switching shortcuts
		} else if (ImGui::IsKeyDown(ImGuiMod_Alt)) {
			if (ImGui::IsKeyPressed(ImGuiKey_0, false)) {
				setSceneCamera(0);

			} else if (ImGui::IsKeyPressed(ImGuiKey_1, false)) {
				setSceneCamera(1);

			} else if (ImGui::IsKeyPressed(ImGuiKey_1, false)) {
				setSceneCamera(1);

			} else if (ImGui::IsKeyPressed(ImGuiKey_2, false)) {
				setSceneCamera(2);

			} else if (ImGui::IsKeyPressed(ImGuiKey_3, false)) {
				setSceneCamera(3);

			} else if (ImGui::IsKeyPressed(ImGuiKey_4, false)) {
				setSceneCamera(4);

			} else if (ImGui::IsKeyPressed(ImGuiKey_5, false)) {
				setSceneCamera(5);

			} else if (ImGui::IsKeyPressed(ImGuiKey_6, false)) {
				setSceneCamera(6);

			} else if (ImGui::IsKeyPressed(ImGuiKey_7, false)) {
				setSceneCamera(7);

			} else if (ImGui::IsKeyPressed(ImGuiKey_8, false)) {
				setSceneCamera(8);

			} else if (ImGui::IsKeyPressed(ImGuiKey_9, false)) {
				setSceneCamera(9);
			}
		}
	}

	if (ImGui::IsKeyPressed(ImGuiKey_F19, false)) {
		toggleRendererDebug();
	}
}


//
//	OtSceneEditor::renderEditor
//

void OtSceneEditor::renderEditor() {
	// update (evaluate) all the nodes
	scene.evaluateNodes();

	// determine button size
	buttonSize = ImGui::GetFrameHeight();

	// render the editor parts
	determinePanelSizes();
	renderPanels();
	OtUi::splitterHorizontal(&panelWidth, minPanelWidth, maxPanelWidth);
	renderViewPort();

	if (nextTask) {
		taskManager.perform(nextTask);

		// special handling for "new entity" task so we can automatically select the new kid on the block
		auto task = std::dynamic_pointer_cast<OtCreateEntityTask>(nextTask);

		if (task) {
			selectedEntity = task->getEntity();
		}

		// clear task so we don't perform it again
		nextTask = nullptr;
	}

	// show the debug window (if required)
	if (showRendererDebug) {
		renderDebug->render(*renderer);
	}
}


//
//	OtSceneEditor::renderPanels
//

void OtSceneEditor::renderPanels() {
	// create a new child window
	ImGui::BeginChild("panels", ImVec2(panelWidth, 0.0f));

	// create the entities panel
	auto spacing = ImGui::GetStyle().ItemSpacing;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	ImGui::BeginChild("entities", ImVec2(0.0f, entityPanelHeight), ImGuiChildFlags_Borders);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing);
	renderEntitiesPanel();
	ImGui::PopStyleVar();
	ImGui::EndChild();
	ImGui::PopStyleVar();

	// render splitter between entity and component panels
	OtUi::splitterVertical(&entityPanelHeight, minEntityPanelHeight, maxEntityPanelHeight);

	// create the components panel
	ImGui::BeginChild("components", ImVec2(), ImGuiChildFlags_Borders);
	renderComponentsPanel();
	ImGui::EndChild();

	// close the panels
	ImGui::EndChild();
}


//
//	OtSceneEditor::renderEntitiesPanel
//

void OtSceneEditor::renderEntitiesPanel() {
	renderPanel(
		"Entities",
		true,
		[this]() {
			// create menu to add entities
			renderNewEntitiesMenu(scene.getRootEntity());
		},
		[this]() {
			// render all root entities
			renderChildEntities(scene.getRootEntity());
		});
}


//
//	OtSceneEditor::renderComponentsPanel
//

void OtSceneEditor::renderComponentsPanel() {
	// render the panel
	renderPanel(
		"Components",
		scene.isValidEntity(selectedEntity),
		[this]() {
			// create menu to add components
			renderNewComponents<OtSceneComponents>();
		},
		[this]() {
			// render component editors if we have a selected entity
			if (scene.isValidEntity(selectedEntity)) {
				renderComponents<OtSceneComponents>();
		}
	});
}


//
//	OtSceneEditor::renderViewPort
//

void OtSceneEditor::renderViewPort() {
	// create the window
	ImGui::BeginChild("viewport", ImVec2(), ImGuiChildFlags_Borders);
	auto size = ImGui::GetContentRegionAvail();

	// unset selected entity if it is no longer valid
	if (!scene.isValidEntity(selectedEntity)) {
		selectedEntity = OtEntityNull;
	}

	// ensure we still have a valid selected camera (no valid scene camera means we use the editor camara)
	if (scene.isValidEntity(selectedCamera)) {
		if (!scene.hasComponent<OtCameraComponent>(selectedCamera) || !scene.hasComponent<OtTransformComponent>(selectedCamera)) {
			selectedCamera = OtEntityNull;
		}

	} else {
		selectedCamera = OtEntityNull;
	}

	// get camera information
	glm::mat4 cameraViewMatrix;
	float nearPlane;
	float farPlane;
	float fov;

	if (scene.isValidEntity(selectedCamera)) {
		glm::mat4 transform = scene.getGlobalTransform(selectedCamera);
		cameraViewMatrix = glm::inverse(transform);

		auto& camera = scene.getComponent<OtCameraComponent>(selectedCamera);
		nearPlane = camera.nearPlane;
		farPlane = camera.farPlane;
		fov = camera.fov;

	} else {
		editorCamera.update();
		cameraViewMatrix = editorCamera.getViewMatrix();
		nearPlane = editorCamera.getNearPlane();
		farPlane = editorCamera.getFarPlane();
		fov = editorCamera.getFov();
	}

	// handle mouse interactions
	auto savedPos = ImGui::GetCursorScreenPos();
	ImGui::Dummy(size);

	if (!renderer->isPicking() && ImGui::IsItemClicked()) {
		auto position = ImGui::GetMousePos() - ImGui::GetItemRectMin();
		glm::vec2 ndc{position.x / size.x * 2.0f - 1.0f, (size.y - position.y) / size.y * 2.0f - 1.0f};

		renderer->pickEntity(ndc, [this](OtEntity entity) {
			selectedEntity = entity;

			if (selectedEntity != OtEntityNull) {
				scrollToSelected = true;
			}
		});
	}

	ImGui::SetCursorScreenPos(savedPos);

	// render the scene
	OtCamera camera{int(size.x), int(size.y), nearPlane, farPlane, fov, cameraViewMatrix};
	renderer->setGridScale(gridEnabled ? gridScale : 0.0f);
	renderer->setSelectedEntity(selectedEntity);
	auto textureIndex = renderer->render(camera, &scene);

	// show it on the screen
	if (OtGpuHasOriginBottomLeft()) {
		ImGui::Image((ImTextureID)(intptr_t) textureIndex, size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

	} else {
		ImGui::Image((ImTextureID)(intptr_t) textureIndex, size);
	}

	// only show guizmo if it's enabled and the selected entity has a transform
	if (guizmoEnabled && scene.isValidEntity(selectedEntity) && scene.hasComponent<OtTransformComponent>(selectedEntity)) {
		// configure the guizmo
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());

		ImGuizmo::SetRect(
			ImGui::GetWindowPos().x,
			ImGui::GetWindowPos().y,
			ImGui::GetWindowWidth(),
			ImGui::GetWindowHeight());

		// get the target transform
		auto& component = scene.getComponent<OtTransformComponent>(selectedEntity);
		auto transform = component.getTransform();

		// show the guizmo
		if(ImGuizmo::Manipulate(
			glm::value_ptr(camera.viewMatrix),
			glm::value_ptr(camera.projectionMatrix),
			guizmoOperation,
			ImGuizmo::LOCAL,
			glm::value_ptr(transform),
			nullptr,
			guizmoSnapping ? glm::value_ptr(guizmoSnapInterval) : nullptr)) {

			// get the old state
			auto oldValue = component.serialize(nullptr).dump();

			// apply guizmo matrix
			ImGuizmo::DecomposeMatrixToComponents(
				glm::value_ptr(transform),
				glm::value_ptr(component.translation),
				glm::value_ptr(component.rotation),
				glm::value_ptr(component.scale));

			// create an edit task so this can be undone
			auto newValue = component.serialize(nullptr).dump();
			nextTask = std::make_shared<OtEditComponentTask<OtTransformComponent>>(&scene, selectedEntity, oldValue, newValue);
		}
	}

	if (!ImGuizmo::IsUsing() && ImGui::IsItemHovered() && !scene.isValidEntity(selectedCamera)) {
		// handle camera interactions
		editorCamera.handleKeyboardAndMouse();
	}

	// let asset manager show busy spinner (if required)
	OtAssetManager::renderUI();
	ImGui::EndChild();
}


//
//	OtSceneEditor::determinePanelSizes
//

void OtSceneEditor::determinePanelSizes() {
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
//	OtSceneEditor::renderPanel
//

void OtSceneEditor::renderPanel(const std::string& name, bool canAdd, std::function<void()> menu, std::function<void()> content) {
	// render a header
	float buttonSpace = canAdd ? buttonSize + ImGui::GetStyle().FramePadding.x : 0.0f;
	OtUi::header(name.c_str(), ImGui::GetContentRegionAvail().x - buttonSpace);

	// add a button to add more things (if required)
	if (canAdd) {
		ImGui::SameLine(ImGui::GetCursorPos().x + ImGui::GetContentRegionAvail().x - buttonSize);
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
	ImGui::PushItemWidth(200.0f);
	content();
	ImGui::PopItemWidth();
	ImGui::EndChild();
}


//
//	OtSceneEditor::renderEntity
//

static inline void* createID(OtEntity entity, uint32_t key) {
	return (void*) ((((uint64_t) key) << 32) | (uint64_t) entity);
}

void OtSceneEditor::renderEntity(OtEntity entity) {
	// determine flags
	ImGuiTreeNodeFlags flags =
	ImGuiTreeNodeFlags_DefaultOpen |
	ImGuiTreeNodeFlags_OpenOnArrow |
	ImGuiTreeNodeFlags_FramePadding |
	ImGuiTreeNodeFlags_SpanAvailWidth |
	ImGuiTreeNodeFlags_AllowOverlap;

	if (entity == selectedEntity) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	if (!scene.hasChildren(entity)) {
		flags |= ImGuiTreeNodeFlags_Bullet;
	}

	// create a tree node
	ImGui::PushID(createID(entity, 1));
	auto tag = scene.getEntityTag(entity);
	bool open = ImGui::TreeNodeEx("##node", flags);
	auto rectMin = ImGui::GetItemRectMin();
	auto rectMax = ImGui::GetItemRectMax();

	// scroll to this entity (if required)
	if (entity == selectedEntity && scrollToSelected) {
		if (!ImGui::IsItemVisible()) {
			ImGui::SetScrollHereY();
		}

		scrollToSelected = false;
	}

	// handle entity selections
	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen() && renamingEntity != entity) {
		selectedEntity = ImGui::GetIO().KeyShift ? OtEntityNull : entity;
		renamingEntity = OtEntityNull;
	}

	// handle double clicks (to start editing entity tag)
	bool startEntityRenaming = false;

	if (entity != renamingEntity && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
		renamingEntity = entity;
		startEntityRenaming = true;
	}

	// entities are drag sources
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		ImGui::SetDragDropPayload("entity", &entity, sizeof(entity));
		ImGui::Text("%s", tag.c_str());
		ImGui::EndDragDropSource();
	}

	// entities are drag targets
	ImGui::PushStyleColor(ImGuiCol_DragDropTarget, 0x8000b0b0);

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("entity")) {
			OtEntity entityToBeMoved;
			std::memcpy(&entityToBeMoved, payload->Data, sizeof(OtEntity));
			nextTask = std::make_shared<OtMoveEntityTask>(&scene, entity, entityToBeMoved, false);
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::PopStyleColor();

	// are we renaming this entity?
	if (entity == renamingEntity) {
		ImGui::SameLine(rectMin.x - ImGui::GetStyle().FramePadding.y - 1);

		ImGui::SetNextItemWidth(
			rectMax.x - rectMin.x -
			ImGui::GetTreeNodeToLabelSpacing() -
			(buttonSize + ImGui::GetStyle().FramePadding.y) * 2.0f);

		auto& component = scene.getComponent<OtCoreComponent>(entity);
		auto oldValue = component.serialize(nullptr).dump();

		if (startEntityRenaming) {
			ImGui::SetKeyboardFocusHere();
		}

		if (OtUi::inputText("##rename", &component.tag)) {
			auto newValue = component.serialize(nullptr).dump();
			nextTask = std::make_shared<OtEditComponentTask<OtCoreComponent>>(&scene, entity, oldValue, newValue);
			renamingEntity = OtEntityNull;

		} else if (ImGui::IsItemDeactivated()) {
			renamingEntity = OtEntityNull;
		}

	} else {
		ImGui::SameLine(rectMin.x);
		ImGui::Text("%s", tag.c_str());
	}

	// is this entity selected
	if (entity == selectedEntity) {
		ImGui::GetWindowDrawList()->AddRect(rectMin, rectMax, 0x8000FF00);
	}

	// add button to remove an entity
	auto& style = ImGui::GetStyle();
	auto right = ImGui::GetCursorPos().x + ImGui::GetContentRegionAvail().x;
	ImGui::SameLine(right - buttonSize * 2.0f - style.FramePadding.y);

	if (ImGui::Button("x##remove", ImVec2(buttonSize, buttonSize))) {
		nextTask = std::make_shared<OtDeleteEntityTask>(&scene, entity);
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
//	OtSceneEditor::renderChildEntities
//

void OtSceneEditor::renderChildEntities(OtEntity entity) {
	// spacing between children is provided by invisible drop targets
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2());

	// process all children
	auto child = scene.getFirstChild(entity);

	while (scene.isValidEntity(child)) {
		// create drop target
		ImGui::PushID(createID(child, 2));
		ImGui::InvisibleButton("##", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetStyle().FramePadding.y));
		ImGui::PushStyleColor(ImGuiCol_DragDropTarget, 0x8000b0b0);

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("entity")) {
				OtEntity entityToBeMoved;
				std::memcpy(&entityToBeMoved, payload->Data, sizeof(OtEntity));
				nextTask = std::make_shared<OtMoveEntityTask>(&scene, child, entityToBeMoved, true);
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::PopStyleColor();
		ImGui::PopID();

		// render the child entity
		renderEntity(child);
		child = scene.getNextSibling(child);
	};

	ImGui::PopStyleVar();
}


//
//	OtSceneEditor::renderNewEntitiesMenu
//

void OtSceneEditor::renderNewEntitiesMenu(OtEntity entity) {
	static struct {
		const char* name;
		OtCreateEntityTask::Type type;
	} predefinedEntities[] = {
		{ "Empty Entity", OtCreateEntityTask::empty},
		{ "Camera", OtCreateEntityTask::camera},
		{ "Directional Light", OtCreateEntityTask::directionalLight},
		{ "Model", OtCreateEntityTask::model},
		{ "Geometry", OtCreateEntityTask::geometry},
		{ "Procedural Sky", OtCreateEntityTask::sky},
		{ "Sky Box", OtCreateEntityTask::skybox},
		{ "Infinite Terrain", OtCreateEntityTask::terrain},
		{ "Procedural Water", OtCreateEntityTask::water},
		{ "Particles", OtCreateEntityTask::particles}
	};

	for (auto& predefined : predefinedEntities) {
		if (ImGui::MenuItem(predefined.name)) {
			nextTask = std::make_shared<OtCreateEntityTask>(&scene, entity, predefined.type);
		}
	}
}


//
//	OtSceneEditor::renderNewComponent
//

template <typename T>
void OtSceneEditor::renderNewComponent() {
	if (ImGui::MenuItem(T::name, nullptr, false, !scene.hasComponent<T>(selectedEntity))) {
		nextTask = std::make_shared<OtCreateComponentTask<T>>(&scene, selectedEntity);
	}
}


//
//	OtSceneEditor::renderComponentEditor
//

template <typename T>
void OtSceneEditor::renderComponentEditor() {
	auto& component = scene.getComponent<T>(selectedEntity);
	auto oldValue = component.serialize(nullptr).dump();

	if (component.renderUI()) {
		auto newValue = component.serialize(nullptr).dump();
		nextTask = std::make_shared<OtEditComponentTask<T>>(&scene, selectedEntity, oldValue, newValue);
	}
}


//
//	OtSceneEditor::renderComponent
//

template <typename T>
void OtSceneEditor::renderComponent() {
	// only render if entity has this component
	if (scene.hasComponent<T>(selectedEntity)) {
		// add a new ID to the stack to avoid collisions
		ImGui::PushID(reinterpret_cast<void*>(typeid(T).hash_code()));

		// create a tree node for the component
		ImGuiTreeNodeFlags flags =
			ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_AllowOverlap;

		bool open = ImGui::TreeNodeEx("##header", flags, T::name);
		bool removeComponent = false;

		// add button to remove the component
		ImGui::SameLine(ImGui::GetCursorPos().x + ImGui::GetContentRegionAvail().x - buttonSize);
		removeComponent = ImGui::Button("x##remove", ImVec2(buttonSize, buttonSize));

		// render the component editor (if required)
		if (open) {
			renderComponentEditor<T>();
			ImGui::TreePop();
		}

		// render a seperator line
		ImGui::Separator();

		// remove component (if required)
		if (removeComponent) {
			nextTask = std::make_shared<OtDeleteComponentTask<T>>(&scene, selectedEntity);
		}

		// clean ID stack
		ImGui::PopID();
	}
}


//
//	OtSceneEditor::cutEntity
//

void OtSceneEditor::cutEntity() {
	nextTask = std::make_shared<OtCutEntityTask>(&scene, selectedEntity, clipboard);
}


//
//	OtSceneEditor::copyEntity
//

void OtSceneEditor::copyEntity() {
	nextTask = std::make_shared<OtCopyEntityTask>(&scene, selectedEntity, clipboard);
}


//
//	OtSceneEditor::pasteEntity
//

void OtSceneEditor::pasteEntity() {
	nextTask = std::make_shared<OtPasteEntityTask>(&scene, selectedEntity, clipboard);
}


//
//	OtSceneEditor::duplicateEntity
//

void OtSceneEditor::duplicateEntity() {
	nextTask = std::make_shared<OtDuplicateEntityTask>(&scene, selectedEntity);
}


//
//	OtSceneEditor::saveEditorCamera
//

void OtSceneEditor::saveEditorCamera(OtEntity entity) {
	// get camera components
	auto& camera = scene.getComponent<OtCameraComponent>(entity);
	auto& transform = scene.getComponent<OtTransformComponent>(entity);
	auto oldCamera = camera.serialize(nullptr).dump();
	auto oldTransform = transform.serialize(nullptr).dump();

	// transfer values
	camera.fov = editorCamera.getFov();
	camera.nearPlane = editorCamera.getNearPlane();
	camera.farPlane = editorCamera.getFarPlane();

	transform.translation = editorCamera.getPosition();
	transform.rotation = glm::vec3(editorCamera.getPitch(), editorCamera.getYaw(), 0.0f);

	// create an edit task so this can be undone
	auto newCamera = camera.serialize(nullptr).dump();
	auto newTransform = transform.serialize(nullptr).dump();
	nextTask = std::make_shared<OtSaveEditorCameraTask>(&scene, entity, oldCamera, oldTransform, newCamera, newTransform);
}


//
//	OtSceneEditor::toggleRendererDebug
//

void OtSceneEditor::toggleRendererDebug() {
	if (!renderDebug) {
		renderDebug = std::make_unique<OtSceneRendererDebug>();
	}

	showRendererDebug = !showRendererDebug;
}
