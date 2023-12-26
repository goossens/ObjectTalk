//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_node_editor.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtGraphEditor.h"
#include "OtMathNodes.h"

#include "OtCreateLinkTask.h"
#include "OtCreateNodeTask.h"
#include "OtDragNodesTask.h"


//
//	Shortcuts
//

namespace ed = ax::NodeEditor;


//
//	Constants
//

static constexpr float nodeWidth = 150.0f;
static constexpr float nodeRounding = 5.0f;
static constexpr float pinRadius = 6.0f;
static constexpr float pinSize = pinRadius * 2.0f;
static constexpr float pinOffsetV = 2.0f;


static constexpr ImU32 creatingLinkColor = 0xff7edef8;
static constexpr ImU32 validLinkColor = 0xff80ff80;
static constexpr ImU32 invalidLinkColor = 0xff8080ff;

static constexpr ImU32 pinColors[] = {
	0xffd7a7ce, // bool
	0xffb3b3b3, // float
	0xff629f60, // vec2
	0xffff7575, // vec3
	0xff29c7c7  // color
};


//
//	OtGraphEditor::OtGraphEditor
//

OtGraphEditor::OtGraphEditor() {
	// create our default context
	ed::Config config;
	config.SettingsFile = nullptr;
	config.NavigateButtonIndex = ImGuiMouseButton_Middle;
	config.CustomZoomLevels.push_back(0.2f);
	config.CustomZoomLevels.push_back(0.3f);
	config.CustomZoomLevels.push_back(0.4f);
	config.CustomZoomLevels.push_back(0.5f);
	config.CustomZoomLevels.push_back(0.6f);
	config.CustomZoomLevels.push_back(0.7f);
	config.CustomZoomLevels.push_back(0.8f);
	config.CustomZoomLevels.push_back(0.9f);
	config.CustomZoomLevels.push_back(1.0f);
	editorContext = ed::CreateEditor(&config);

	ed::SetCurrentEditor(editorContext);
	ed::EnableShortcuts(false);
	auto& style = ed::GetStyle();
	style.NodeRounding = nodeRounding;
	nodePadding = style.NodePadding;
	nodeBorderWidth = style.NodeBorderWidth;
	ed::SetCurrentEditor(nullptr);

	graph = std::make_unique<OtGraph>();
	OtMathNodesRegister(*graph);

	graph->createNode("Add", 100.0f, 100.0f);
	graph->createNode("Subtract", 400.0f, 100.0f);
	graph->createNode("Sin", 100.0f, 300.0f);
}


//
//	OtGraphEditor::load
//

void OtGraphEditor::load() {
	// load graph from file
	nlohmann::json metadata;
	graph->load(path, &metadata);
}


//
//	OtGraphEditor::save
//

void OtGraphEditor::save() {
	// build metadata
	auto metadata = nlohmann::json::object();

	// write scene to file
	graph->save(path, &metadata);
	taskManager.baseline();
}


//
//	OtGraphEditor::isDirty
//

bool OtGraphEditor::isDirty() {
	return taskManager.isDirty();
}


//
//	OtGraphEditor::cutNodes
//

void OtGraphEditor::cutNodes() {
}


//
//	OtGraphEditor::copyNodes
//

void OtGraphEditor::copyNodes() {
}


//
//	OtGraphEditor::pasteNodes
//

void OtGraphEditor::pasteNodes() {
}


//
//	OtGraphEditor::duplicateNodes
//

void OtGraphEditor::duplicateNodes() {
}


//
//	OtGraphEditor::create
//

std::shared_ptr<OtGraphEditor> OtGraphEditor::create(const std::filesystem::path& path) {
	std::shared_ptr<OtGraphEditor> editor = std::make_shared<OtGraphEditor>();
	editor->setFilePath(path);

	if (editor->fileExists()) {
		editor->load();
	}

	return editor;
}


//
//	OtGraphEditor::renderMenu
//

void OtGraphEditor::renderMenu() {
	// get status
	bool selected = areNodesSelected();
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
			if (ImGui::MenuItem("Cut", OT_UI_SHORTCUT "X", nullptr, selected)) { cutNodes(); }
			if (ImGui::MenuItem("Copy", OT_UI_SHORTCUT "C", nullptr, selected)) { copyNodes(); }
			if (ImGui::MenuItem("Paste", OT_UI_SHORTCUT "V", nullptr, selected && clipable)) { pasteNodes(); }
			if (ImGui::MenuItem("Duplicate", OT_UI_SHORTCUT "D", nullptr, selected)) { duplicateNodes(); }
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	// handle keyboard shortcuts
	if (ImGui::IsKeyDown(ImGuiMod_Shortcut)) {
		if (ImGui::IsKeyDown(ImGuiMod_Shift) && ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
			if (taskManager.canRedo()) {
				taskManager.redo();
			}

		} else if (ImGui::IsKeyPressed(ImGuiKey_Z, false) && taskManager.canUndo()) {
			taskManager.undo();

		} else if (ImGui::IsKeyPressed(ImGuiKey_X, false) && selected) {
			cutNodes();

		} else if (ImGui::IsKeyPressed(ImGuiKey_C, false) && selected) {
			copyNodes();

		} else if (ImGui::IsKeyPressed(ImGuiKey_V, false) && selected && clipable) {
			pasteNodes();

		} else if (ImGui::IsKeyPressed(ImGuiKey_D, false) && selected) {
			duplicateNodes();
		}
	}
}


//
//	OtGraphEditor::renderEditor
//

void OtGraphEditor::renderEditor(bool active) {
	// setup node editor
	ed::Begin("GraphEditor");

	// render all nodes
	graph->eachNode([&] (OtGraphNode& node) {
		renderNode(node);
	});

	// render all links
	graph->eachLink([&] (OtGraphLink& link) {
		ed::Link(link->id, link->from->id, link->to->id);
	});

	// handle all interactions
	handleInteractions();

	// perform editing task (if required)
	if (nextTask) {
		taskManager.perform(nextTask);
		nextTask = nullptr;
	}

	// close node editor
	ed::End();
}


//
//	OtGraphEditor::renderNode
//

void OtGraphEditor::renderNode(OtGraphNode& node) {
	// start node rendering
	ed::BeginNode(node->id);
	auto outerSize = ed::GetNodeSize(node->id);
	auto innerSize = outerSize - ImVec2(nodePadding.x * 2.0f, nodePadding.y * 2.0f);

	// set position (if required)
	if (node->needsPlacement) {
		ed::SetNodePosition(node->id, ImVec2(node->x, node->y));
		node->needsPlacement = false;
	}

	// show a title
	auto drawList = ImGui::GetWindowDrawList();
	auto topLeft = ImGui::GetCursorScreenPos() - ImVec2(nodePadding.x, nodePadding.y);
	auto bottomRight = topLeft + ImVec2(outerSize.x, ImGui::GetFrameHeight() + nodePadding.y);

	drawList->AddRectFilled(
		topLeft + ImVec2(nodeBorderWidth, nodeBorderWidth),
		bottomRight - ImVec2(nodeBorderWidth, 0.0f),
		ImGui::GetColorU32(ImGuiCol_Tab),
		nodeRounding);

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (innerSize.x - ImGui::CalcTextSize(node->name).x) / 2.0f);
	ImGui::TextUnformatted(node->name);
	ImGui::Dummy(ImVec2(nodeWidth, nodePadding.y));

	// render all output pins
	node->eachOutput([&] (OtGraphPin& pin) {
		renderPin(pin, bottomRight.x);
	});

	// render all input pins
	node->eachInput([&] (OtGraphPin& pin) {
		renderPin(pin, topLeft.x);
	});

	ed::EndNode();
}


//
//	OtGraphEditor::renderPin
//

void OtGraphEditor::renderPin(OtGraphPin& pin, float x) {
	ed::BeginPin(pin->id, pin->isInput() ? ed::PinKind::Input : ed::PinKind::Output);

	auto drawList = ImGui::GetWindowDrawList();
	auto color = pinColors[pin->type];
	auto center = ImVec2(x, ImGui::GetCursorScreenPos().y + pinRadius + pinOffsetV);
	drawList->AddCircleFilled(center, pinRadius, color, 12);
	ed::PinRect(center - ImVec2(pinRadius, pinRadius), center + ImVec2(pinRadius, pinRadius));

	if (pin->isOutput()) {
		ImGui::Indent(nodeWidth - ImGui::CalcTextSize(pin->name).x);
	}

	ImGui::TextUnformatted(pin->name);
	ed::EndPin();
}


//
//	getPin
//

static OtGraphPin getPin(OtGraph& graph, ed::PinId pinId) {
	return graph.getPin((uint32_t) pinId.Get());
}


//
//	OtGraphEditor::handleInteractions
//

void OtGraphEditor::handleInteractions() {
	// see if we are creating a new link
	if (ed::BeginCreate(ImGui::ColorConvertU32ToFloat4(creatingLinkColor))) {
		ed::PinId startPinId, endPinId;

		if (ed::QueryNewLink(&startPinId, &endPinId)) {
			// validate link
			auto startPin = getPin(*graph, startPinId);
			auto endPin = getPin(*graph, endPinId);

			if (startPin->isInput()) {
				std::swap(startPin, endPin);
			}

			if (!graph->isLinkValid(startPin, endPin)) {
				ed::RejectNewItem(ImGui::ColorConvertU32ToFloat4(invalidLinkColor));

			} else if (ed::AcceptNewItem(ImGui::ColorConvertU32ToFloat4(validLinkColor))) {
				nextTask = std::make_shared<OtCreateLinkTask>(graph.get(), startPin->id, endPin->id);
			}
		}
	}

	ed::EndCreate();

	// handle node dragging
	if (ImGui::IsMouseDragging(ImGuiMouseButton_Left, 1)) {
		if (!draggedNodeId) {
			auto nodeId = ed::GetHoveredNode();

			if (nodeId) {
				draggedNodeId = (uint32_t) nodeId.Get();
			}
		}

	} else if (draggedNodeId) {
		// stopped dragging node(s), generate next task
		auto selected = getSelectedNodes(draggedNodeId);
		nextTask = std::make_shared<OtDragNodesTask>(graph.get(), selected);
		draggedNodeId = 0;
	}

	// handle context menus
	ed::Suspend();

	if (ed::ShowBackgroundContextMenu()) {
		ImGui::OpenPopup("Background Context");
	}

	if (ImGui::BeginPopup("Background Context")) {
		auto nodePosition = ed::ScreenToCanvas(ImGui::GetMousePosOnOpeningCurrentPopup());
		ImGui::TextUnformatted("Create Node");
		ImGui::Separator();

		auto factory = graph->getNodeFactory();

		factory.eachCategory([&] (OtGraphNodeCategory& category) {
			if (ImGui::BeginMenu(category.name.c_str())) {
				category.eachType([&] (OtGraphNodeType& type) {
					if (ImGui::MenuItem(type.name.c_str())) {
						nextTask = std::make_shared<OtCreateNodeTask>(
							graph.get(),
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

	ed::Resume();
}


//
//	OtGraphEditor::areNodesSelected
//

bool OtGraphEditor::areNodesSelected() {
	std::vector<ed::NodeId> nodes;
	nodes.resize(ed::GetSelectedObjectCount());
	return ed::GetSelectedNodes(nodes.data(), nodes.size()) != 0;
}


//
//	OtGraphEditor::getSelectedNodes
//

std::vector<uint32_t> OtGraphEditor::getSelectedNodes(uint32_t id) {
	std::vector<ed::NodeId> nodes;
	nodes.resize(ed::GetSelectedObjectCount());
	size_t selected = ed::GetSelectedNodes(nodes.data(), nodes.size());
	std::vector<uint32_t> result;

	for (auto i = 0; i < selected; i++) {
		result.push_back((uint32_t) nodes[i].Get());
	}

	if (id && (!selected || std::find(result.begin(), result.end(), id) == result.end())) {
		result.push_back(id);
	}

	return result;
}
