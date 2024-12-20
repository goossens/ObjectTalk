//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtNumbers.h"

#include "OtUi.h"

#include "OtNodesWidget.h"


//
//	Constants
//

static constexpr ImU32 gridColor = IM_COL32(200, 200, 200, 40);
static constexpr ImU32 rubberBandBackgroundColor = IM_COL32(5, 130, 255,  64);
static constexpr ImU32 rubberBandOutlineColor = IM_COL32(5, 130, 255, 192);
static constexpr ImU32 nodeBackgroundColor = IM_COL32(60, 60, 60, 128);
static constexpr ImU32 nodeOutlineColor = IM_COL32(100, 100, 100, 255);
static constexpr ImU32 nodeSelectedColor = IM_COL32(255, 176,  50, 255);
static constexpr ImU32 normalLinkColor = IM_COL32(255, 255, 255, 255);
static constexpr ImU32 creatingLinkColor = IM_COL32(248, 222, 126, 255);
static constexpr ImU32 validLinkColor = IM_COL32(128, 255, 128, 255);
static constexpr ImU32 invalidLinkColor = IM_COL32(255, 32, 32, 255);

static constexpr ImU32 nodeColors[] = {
	IM_COL32(125, 45, 75, 255),		// input
	IM_COL32(60, 30, 40, 255),		// output
	IM_COL32(150, 60, 110, 255),	// field
	IM_COL32(55, 95, 130, 255),		// math
	IM_COL32(50, 100, 40, 255),		// generator
	IM_COL32(115, 70, 30, 255),		// filter
	IM_COL32(35, 60, 130, 255),		// transform
	IM_COL32(80, 130, 170, 255),	// shape
	IM_COL32(30, 110, 90, 255),		// geometry
	IM_COL32(10, 80, 80, 255),		// virtualizer
	IM_COL32(120, 115, 55, 255),	// save
	IM_COL32(50, 50, 90, 255)		// probe
};

static constexpr ImU32 pinColors[] = {
	IM_COL32(220, 130, 0, 255),		// bool
	IM_COL32(100, 160, 100, 255),	// int
	IM_COL32(180, 180, 180, 255),	// float
	IM_COL32(0, 100, 180, 255),		// string
	IM_COL32(120, 60, 255, 200),	// vector
	IM_COL32(200, 51, 123, 255), 	// color
	IM_COL32(140, 190, 40, 255),	// font
	IM_COL32(200, 190, 120, 255),	// image
	IM_COL32(180, 90, 0, 255),		// texture
	IM_COL32(110, 140, 190, 255),	// shape
	IM_COL32(0, 120, 65, 255),		// geometry
	IM_COL32(0, 160, 220, 255)		// instances
};

static constexpr float gridSpacing = 64.0f;
static constexpr float nodeRounding = 4.0f;

static constexpr float pinRadius = 6.0f;
static constexpr float pinBox = pinRadius * 2.0f + 2.0f;
static constexpr float topPadding = 2.0f;
static constexpr float horizontalPadding = pinRadius + 4.0f;

static constexpr float linkThinkness = 1.5f;


//
//	Helper functions
//

static inline void inset(float x) {
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x);
}


//
//	OtNodesWidget::render
//

void OtNodesWidget::render(OtNodes* n) {
	// reset state
	nodes = n;
	hoveredNode = 0;
	hoveredInNodeContent = false;
	hoveredPin = 0;
	pinLocations.clear();

	// start rendering
	ImGuiChildFlags flags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse;

	ImGui::BeginChild("nodes", ImVec2(), ImGuiChildFlags_None, flags);
	position = ImGui::GetCursorScreenPos();
	size = ImGui::GetContentRegionAvail();
	offset = position + scrollingOffset;
	pinOffset = ImGui::GetStyle().FramePadding.y + ImGui::GetFont()->Ascent - pinRadius + 1.0f;
	ImDrawList* drawlist = ImGui::GetWindowDrawList();

	// render the grid and rubberband
	renderGrid(drawlist);
	renderRubberBand(drawlist);

	// render each node
	drawlist->ChannelsSplit(2);
	drawlist->ChannelsSetCurrent(1);

	nodes->eachNode([&](OtNode& node) {
		renderNode(drawlist, node);

		if (node->needsSaving) {
			node->needsSaving = false;
			editedNode = node->id;
			nodeEdited = true;
		}
	});

	// render each link
	drawlist->ChannelsSetCurrent(0);

	nodes->eachLink([&](OtNodesLink& link) {
		if (link->id != ignoreLink) {
			renderLink(
				drawlist,
				pinLocations[link->from->id],
				pinLocations[link->to->id],
				pinColors[link->from->type]);
		}
	});

	// handle user interactions (if required)
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
		ImVec2 mousePos = ImGui::GetMousePos();

		bool inside =
			mousePos.x > position.x &&
			mousePos.x < position.x + size.x &&
			mousePos.y > position.y &&
			mousePos.y < position.y + size.y;

		if (inside || interactionState != noInteraction) {
			handleInteractions(drawlist);
		}

		// are we in the process of connecting nodes?
		if (interactionState == connecting || interactionState == reconnecting) {
			if (outputToInput) {
				renderLink(drawlist, fromPinPos, toPinPos, linkColor);

			} else {
				renderLink(drawlist, toPinPos, fromPinPos, linkColor);
			}
		}
	}

	// we should be done rendering now
	drawlist->ChannelsMerge();
	ImGui::EndChild();
}


//
//	OtNodesWidget::isNodeEdited
//

bool OtNodesWidget::isNodeEdited(uint32_t& node) {
	if (nodeEdited) {
		node = editedNode;
		nodeEdited = false;
		return true;

	} else {
		return false;
	}
}


//
//	OtNodesWidget::isCreatingLink
//

bool OtNodesWidget::isCreatingLink(uint32_t& from, uint32_t& to) {
	if (connectingDone) {
		if (outputToInput) {
			from = fromPin;
			to = toPin;

		} else {
			from = toPin;
			to = fromPin;

		}

		connectingDone = false;
		return true;

	} else {
		return false;
	}
}


//
//	OtNodesWidget::isDroppingLink
//

bool OtNodesWidget::isDroppingLink(uint32_t& from, uint32_t& to) {
	if (disconnectingDone) {
		from = fromPin;
		to = oldToPin;
		disconnectingDone = false;
		return true;

	} else {
		return false;
	}
}


//
//	OtNodesWidget::isChangingLink
//

bool OtNodesWidget::isChangingLink(uint32_t& from, uint32_t& oldTo, uint32_t& newTo) {
	if (reconnectingDone) {
		from = fromPin;
		oldTo = oldToPin;
		newTo = toPin;
		reconnectingDone = false;
		return true;

	} else {
		return false;
	}
}


//
//	OtNodesWidget::isDraggingComplete
//

bool OtNodesWidget::isDraggingComplete(ImVec2& offset)
{
	if (draggingDone) {
		offset = draggedOffset;
		draggingDone = false;
		return true;

	} else {
		return false;
	}
}


//
//	OtNodesWidget::showContextMenu
//

bool OtNodesWidget::showContextMenu() {
	if (contextMenuDone) {
		contextMenuDone = false;
		return true;

	} else {
		return false;
	}
}


//
//	OtNodesWidget::screenToWidget
//

ImVec2 OtNodesWidget::screenToWidget(const ImVec2& in) {
	return in - offset;
}


//
//	OtNodesWidget::renderGrid
//

void OtNodesWidget::renderGrid(ImDrawList* drawlist) {
	for (float x = std::fmod(scrollingOffset.x, gridSpacing); x < size.x; x += gridSpacing) {
		drawlist->AddLine(ImVec2(x, 0.0f) + position, ImVec2(x, size.y) + position, gridColor);
	}

	for (float y = std::fmod(scrollingOffset.y, gridSpacing); y < size.y; y += gridSpacing) {
		drawlist->AddLine(ImVec2(0.0f, y) + position, ImVec2(size.x, y) + position, gridColor);
	}

	drawlist->AddRect(position, position + size, gridColor);
}


//
//	OtNodesWidget::renderRubberBand
//

void OtNodesWidget::renderRubberBand(ImDrawList* drawlist) {
	// render rubberband
	if (interactionState == rubberBand) {
		drawlist->AddRectFilled(rubberBandTopLeft, rubberBandBottomRight, rubberBandBackgroundColor);
		drawlist->AddRect(rubberBandTopLeft, rubberBandBottomRight, rubberBandOutlineColor);
	}
}


//
//	OtNodesWidget::renderNode
//

void OtNodesWidget::renderNode(ImDrawList* drawlist, OtNode& node) {
	// update node size if required
	if (node->needsSizing) {
		calculateNodeSize(node);
		node->needsSizing = false;
	}

	// start the node rendering
	ImGui::PushID(node->id);
	ImVec2 topLeft = offset + ImVec2(node->x, node->y);

	// handle dragging offset
	if (interactionState == dragNodes && node->selected) {
		topLeft += draggingOffset;
	}

	ImVec2 nodeSize = ImVec2(node->w, node->h);
	ImVec2 bottomRight = topLeft + nodeSize;
	ImVec2 headerBottomRight = ImVec2(bottomRight.x, topLeft.y + ImGui::GetFrameHeight() + topPadding);

	// render the node's background
	drawlist->AddRectFilled(topLeft, bottomRight, nodeBackgroundColor, nodeRounding);
	drawlist->AddRectFilled(topLeft, headerBottomRight, nodeColors[node->category], nodeRounding);
	drawlist->AddRect(topLeft, bottomRight, node->selected ? nodeSelectedColor : nodeOutlineColor, nodeRounding);

	// is mouse over node?
	if (OtUi::isMouseInRect(topLeft, bottomRight)) {
		// yes, remember the ID
		hoveredNode = node->id;
		hoveredInNodeContent = true;
	}

	// handle mouse interactions with header
	bool startNodeRenaming = false;
	ImGui::SetCursorScreenPos(topLeft);
	ImGui::InvisibleButton("", headerBottomRight - topLeft);

	if (ImGui::IsItemHovered()) {
		// the header doesn't count as the node content area
		hoveredInNodeContent = false;

		// handle double clicks (to start editing node title)
		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			if (renamingNode != node->id) {
				renamingNode = node->id;
				startNodeRenaming = true;
			}
		}
	}

	// render title
	ImGui::SetCursorScreenPos(topLeft + ImVec2(horizontalPadding, topPadding));
	ImGui::BeginGroup();
	inset(((node->w - horizontalPadding * 2.0f) - ImGui::CalcTextSize(node->title.c_str()).x) / 2.0f);
	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(renamingNode == node->id ? "" : node->title.c_str());

	// render all output pins
	node->eachOutput([&](OtNodesPin& pin) {
		renderPin(drawlist, pin, bottomRight.x, node->w);
	});

	// do the custom node rendering
	node->customRendering(node->w - horizontalPadding * 2.0f);

	// render all input pins
	node->eachInput([&](OtNodesPin& pin) {
		renderPin(drawlist, pin, topLeft.x, node->w);
	});

	ImGui::EndGroup();

	if (node->id == renamingNode) {
		ImGui::SetCursorScreenPos(topLeft + ImVec2(horizontalPadding, topPadding));
		ImGui::SetNextItemWidth(node->w - 2.0 * horizontalPadding);

		if (startNodeRenaming) {
			ImGui::SetKeyboardFocusHere();
			node->oldState = node->serialize().dump();
		}

		if (OtUi::inputText("##rename", &node->title)) {
			node->newState = node->serialize().dump();
			editedNode = node->id;
			nodeEdited = true;
			renamingNode = 0;

		} else if (ImGui::IsItemDeactivated()) {
			renamingNode = 0;
		}

		// don't know why this is required in this case
		if (ImGui::IsItemHovered()) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
		}
	}

	ImGui::PopID();
}


//
//	OtNodesWidget::renderPin
//

void OtNodesWidget::renderPin(ImDrawList* drawlist, OtNodesPin& pin, float x, float w) {
	// render the pin
	auto color = pinColors[pin->type];
	auto pos = ImVec2(x, ImGui::GetCursorScreenPos().y + pinOffset);

	if (pin->isVarying()) {
		drawlist->AddQuadFilled(
			ImVec2(pos.x, pos.y - pinRadius - 1),
			ImVec2(pos.x + pinRadius + 1, pos.y),
			ImVec2(pos.x, pos.y + pinRadius + 1),
			ImVec2(pos.x - pinRadius - 1, pos.y),
			color);

	} else {
		drawlist->AddCircleFilled(pos, pinRadius, color, 12);
	}

	// remember pin location
	pinLocations[pin->id] = pos;

	// handle mouse interactions
	auto savedPos = ImGui::GetCursorScreenPos();
	ImGui::SetCursorScreenPos(pos - ImVec2(pinBox / 2.0f, pinBox / 2.0f));
	ImGui::Dummy(ImVec2(pinBox, pinBox));

	if (ImGui::IsItemHovered()) {
		hoveredPin = pin->id;
	}

	ImGui::SetCursorScreenPos(savedPos);

	// see if we have a custom renderer
	if (pin->hasRenderer) {
		pin->render(w - horizontalPadding * 2.0f);

	} else {
		// right align labels for output pins
		if (pin->isOutput()) {
			inset(w - ImGui::CalcTextSize(pin->name).x - horizontalPadding * 2.0f);
		}

		// render label
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(pin->name);

		// show UI for unconnected input pins that have that capability
		if (pin->isInput() && !pin->isSourceConnected() && pin->inputConfig) {
			ImGui::SameLine(0.0f, horizontalPadding);
			auto width = w - horizontalPadding * 3.0f - ImGui::CalcTextSize(pin->name).x;
			pin->inputConfig->editValue(pin, width);
		}
	}
}


//
//	OtNodesWidget::renderLink
//

void OtNodesWidget::renderLink(ImDrawList* drawlist, const ImVec2& start, const ImVec2& end, ImU32 color) {
	// see what kind of line we need
	if (start.x < end.x) {
		// we need a simple left to right curved line
		auto distanceX = end.x - start.x;
		auto distanceY = end.y - start.y;
		auto length = std::sqrt(distanceX * distanceX + distanceY * distanceY);
		auto offset = ImVec2(0.25f * length, 0.0f);
		drawlist->AddBezierCubic(start, start + offset, end - offset, end, color, linkThinkness);

	} else {
		// we need a wrap-around curved line
		float vd2 = (end.y - start.y) / 2.0f;
		float vd2a = std::abs(vd2);
		ImVec2 center1 = start + ImVec2(0.0f, vd2);
		ImVec2 center2 = end - ImVec2(0.0f, vd2);
		ImVec2 control1a = start + ImVec2(vd2a, 0.0f);
		ImVec2 control1b = start + ImVec2(vd2a, vd2);
		ImVec2 control2a = end - ImVec2(vd2a, vd2);
		ImVec2 control2b = end - ImVec2(vd2a, 0.0f);
		drawlist->AddBezierCubic(start, control1a, control1b, center1, color, linkThinkness);
		drawlist->AddLine(center1, center2, color, linkThinkness);
		drawlist->AddBezierCubic(center2, control2a, control2b, end, color, linkThinkness);
	}
}


//
//	OtNodesWidget::calculateNodeSize
//

void OtNodesWidget::calculateNodeSize(OtNode& node) {
	// determine widest line
	// first we look at node title width
	auto w = ImGui::CalcTextSize(node->title.c_str()).x;

	// then we look at each pin
	node->eachPin([&](OtNodesPin& pin) {
		if (pin->hasRenderer) {
			// use dimensions for custom pin rendering
			w = std::max(w, pin->renderingWidth);

		} else if (pin->isInput() && !pin->isSourceConnected() && pin->inputConfig) {
			// use pin label width, spacer and input field width
			w = std::max(w, ImGui::CalcTextSize(pin->name).x + horizontalPadding + pin->inputConfig->getMinimumSize());

		} else {
			// just use pin label width
			w = std::max(w, ImGui::CalcTextSize(pin->name).x);
		}
	});

	// take any node custom rendering into account
	w = std::max(w, node->getCustomRenderingWidth());
	node->w = w + horizontalPadding * 2.0f;

	// determine height
	node->h =
		topPadding +
		ImGui::GetFrameHeightWithSpacing() +
		ImGui::GetFrameHeightWithSpacing() * node->getOutputPinCount() +
		node->getCustomRenderingHeight() +
		ImGui::GetFrameHeightWithSpacing() * node->getInputPinCount();
}


//
//	OtNodesWidget::handleInteractions
//

void OtNodesWidget::handleInteractions(ImDrawList* drawlist) {
	// see if we have the start of a new mouse interaction
	if (interactionState == noInteraction) {
		// handle left mouse button events
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			// are we hitting a pin?
			if (hoveredPin) {
				auto pin = nodes->getPin(hoveredPin);

				// for an output pin we handle a new connection
				if (pin->isOutput()) {
					fromPin = hoveredPin;
					fromPinPos = pinLocations[pin->id];
					toPin = 0;
					toPinPos = fromPinPos;
					outputToInput = true;
					interactionState = connecting;

				// for a connected input pin we handle a (re/dis)connect
				} else if (pin->isSourceConnected()) {
					fromPin = pin->getSource()->id;
					fromPinPos = pinLocations[fromPin];
					oldToPin = pin->id;
					toPin = 0;
					toPinPos = pinLocations[pin->id];
					ignoreLink = nodes->findLink(fromPin, oldToPin)->id;
					outputToInput = true;
					interactionState = reconnecting;

				// for an unconnected input pin we handle a new connection
				} else {
					fromPin = hoveredPin;
					fromPinPos = pinLocations[pin->id];
					toPin = 0;
					toPinPos = fromPinPos;
					outputToInput = false;
					interactionState = connecting;
				}

			// are we hitting a node?
			} else if (hoveredNode) {
				// yes, is it the content area (body)?
				if (hoveredInNodeContent) {
					// ignore mouse events until mouse is released
						interactionState = ignoreMouse;

				} else {
					// we hit the header, handle node selection
					if (!nodes->isNodeSelected(hoveredNode)) {
						nodes->select(hoveredNode, !ImGui::IsKeyDown(ImGuiMod_Shift));
					}

					interactionState = selectNode;
				}

			// we hitting the background
			} else {
				// start rubber banding
				rubberBandStartPos = ImGui::GetMousePos();
				rubberBandTopLeft = rubberBandStartPos;
				rubberBandBottomRight = rubberBandStartPos;
				interactionState = rubberBand;
			}

		// handle right mouse button events
		} else if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
			if (!hoveredPin && !hoveredNode) {
				contextMenuDone = true;
			}

		// handle middle mouse button events
		} else if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
			scrollingStartPos = ImGui::GetMousePos() - scrollingOffset;
			interactionState = scrolling;
		}

	// handle ignore mouse state
	} else if (interactionState == ignoreMouse) {
		if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			interactionState = noInteraction;
		}

	// handle select node state
	} else if (interactionState == selectNode) {
		// end state if required
		if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			interactionState = noInteraction;

		// see if we started dragging
		} else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
			draggingStartPos = ImGui::GetMousePos();
			interactionState = dragNodes;
		}

	// handle drag node state
	} else if (interactionState == dragNodes) {
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			draggingOffset = ImGui::GetMousePos() - draggingStartPos;

		} else {
			draggingDone = true;
			draggedOffset = draggingOffset;
			draggingOffset = ImVec2();
			interactionState = noInteraction;
		}

	// handle rubberband state
	} else if (interactionState == rubberBand) {
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			auto pos = ImGui::GetMousePos();

			rubberBandTopLeft = ImVec2(
				std::min(pos.x, rubberBandStartPos.x),
				std::min(pos.y, rubberBandStartPos.y));

			rubberBandBottomRight = ImVec2(
				std::max(pos.x, rubberBandStartPos.x),
				std::max(pos.y, rubberBandStartPos.y));

			auto topLeft = rubberBandTopLeft - offset;
			auto bottomRight = rubberBandBottomRight - offset;
			nodes->select(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);

		} else {
			interactionState = noInteraction;
		}

	// handle nodes scrolling
	} else if (interactionState == scrolling) {
		if (ImGui::IsMouseDown(ImGuiPopupFlags_MouseButtonMiddle)) {
			scrollingOffset = ImGui::GetMousePos() - scrollingStartPos;

		} else {
			interactionState = noInteraction;
		}

	// handle node connection (create a new link)
	} else if (interactionState == connecting) {
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			// are we hovering over another pin?
			if (hoveredPin && hoveredPin != fromPin) {
				toPin = hoveredPin;
				toPinPos = pinLocations[hoveredPin];

				if (outputToInput) {
					linkValid = nodes->isLinkValid(fromPin, toPin);

				} else {
					linkValid = nodes->isLinkValid(toPin, fromPin);
				}

				linkColor = linkValid ? validLinkColor : invalidLinkColor;

			} else {
				toPin = 0;
				toPinPos = ImGui::GetMousePos();
				linkValid = false;
				linkColor = creatingLinkColor;
			}

		} else {
			if (linkValid) {
				connectingDone = true;
			}

			interactionState = noInteraction;
		}

	// handle node reconnection (change an existing link)
	} else if (interactionState == reconnecting) {
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			// are we hovering over another pin?
			if (hoveredPin) {
				toPin = hoveredPin;
				toPinPos = pinLocations[hoveredPin];

				if (toPin == oldToPin) {
					linkValid = true;

				} else {
					linkValid = nodes->isLinkValid(fromPin, toPin);
				}

				linkColor = linkValid ? validLinkColor : invalidLinkColor;

			} else {
				toPin = 0;
				toPinPos = ImGui::GetMousePos();
				linkValid = false;
				linkColor = creatingLinkColor;
			}

		} else {
			if (linkValid) {
				if (toPin != oldToPin) {
					reconnectingDone = true;
				}

			} else {
				disconnectingDone = true;
			}

			ignoreLink = 0;
			interactionState = noInteraction;
		}
	}

	// handle keyboard interactions
	if (interactionState == noInteraction) {
		if (ImGui::IsKeyPressed(ImGuiKey_Home)) {
			scrollingOffset = ImVec2();
		}
	}
}
