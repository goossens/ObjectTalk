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
	config.CustomZoomLevels.push_back(0.2f);
	config.CustomZoomLevels.push_back(0.4f);
	config.CustomZoomLevels.push_back(0.6f);
	config.CustomZoomLevels.push_back(0.8f);
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
	graph->createNode("Subtract", 300.0f, 100.0f);
	graph->createNode("Sin", 100.0f, 300.0f);
}


//
//	OtGraphEditor::load
//

void OtGraphEditor::load() {
	// load graph from file
	nlohmann::json metadata;
	graph->load(path, &metadata);
	firstFrame = true;
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
	bool selected = selectedNodes.size() != 0;
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
}


//
//	getPinID
//

static OtGraphPin getPinID(OtGraph& graph, ed::PinId pinId) {
	return graph.getPin((int) pinId.Get());
}


//
//	OtGraphEditor::renderEditor
//

void OtGraphEditor::renderEditor(bool active) {
	// setup node editor
	ed::SetCurrentEditor(editorContext);
	ed::Begin("GraphEditor");

	// render all nodes
	graph->eachNode([&] (OtGraphNode& node) {
		renderNode(node);
	});

	// render all links
	graph->eachLink([&] (OtGraphLink& link) {
		ed::Link(link.id, link.from->id, link.to->id);
	});

	// handle all interactions
	handleInteractions();

	// close node editor
	ed::End();
	ed::SetCurrentEditor(nullptr);
	firstFrame = false;
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
	if (firstFrame) {
		ed::SetNodePosition(node->id, ImVec2(node->x, node->y));
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
//	OtGraphEditor::handleInteractions
//

void OtGraphEditor::handleInteractions() {
	// check for interactions
	if (ed::BeginCreate()) {
		ed::PinId startPinId, endPinId;

		if (ed::QueryNewLink(&startPinId, &endPinId)) {
			// validate link
			auto startPin = getPinID(*graph, startPinId);
			auto endPin = getPinID(*graph, endPinId);

			if (startPin->isInput()) {
				std::swap(startPin, endPin);
			}

			if (!graph->isLinkValid(startPin, endPin)) {
				ed::RejectNewItem(ImGui::ColorConvertU32ToFloat4(invalidLinkColor));

			} else if (ed::AcceptNewItem(ImGui::ColorConvertU32ToFloat4(validLinkColor))) {
				int linkID = graph->createLink(startPin, endPin);

//				m_Links.push_back({ ed::LinkId(m_NextLinkId++), inputPinId, outputPinId });
//				ed::Link(m_Links.back().Id, m_Links.back().InputId, m_Links.back().OutputId);
			}
		}
	}

	ed::EndCreate();
}
