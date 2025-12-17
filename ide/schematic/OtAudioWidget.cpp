//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>
#include <vector>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtException.h"
#include "OtLog.h"
#include "OtNumbers.h"

#include "OtUi.h"

#include "OtAudioWidget.h"


//
//	Constants
//

static constexpr ImU32 gridColor = IM_COL32(200, 200, 200, 40);
static constexpr ImU32 rubberBandBackgroundColor = IM_COL32(5, 130, 255,  64);
static constexpr ImU32 rubberBandOutlineColor = IM_COL32(5, 130, 255, 192);
static constexpr ImU32 circuitBackgroundColor = IM_COL32(60, 60, 60, 128);
static constexpr ImU32 circuitOutlineColor = IM_COL32(100, 100, 100, 255);
static constexpr ImU32 circuitSelectedColor = IM_COL32(255, 176,  50, 255);
static constexpr ImU32 creatingWireColor = IM_COL32(248, 222, 126, 255);
static constexpr ImU32 validWireColor = IM_COL32(128, 255, 128, 255);
static constexpr ImU32 invalidWireColor = IM_COL32(255, 32, 32, 255);
static constexpr ImU32 errorBackgroundColor = IM_COL32(255, 32, 32, 64);
static constexpr ImU32 errorTooltipColor = IM_COL32(128, 0, 32, 255);

static constexpr ImU32 circuitColors[] = {
	IM_COL32(125, 45, 75, 255),		// input
	IM_COL32(60, 30, 40, 255),		// output
	IM_COL32(150, 60, 110, 255),	// generator
	IM_COL32(55, 95, 130, 255),		// effect
	IM_COL32(50, 50, 90, 255)		// probe
	};

static constexpr ImU32 pinColors[] = {
	IM_COL32(220, 130, 0, 255),		// mono
	IM_COL32(100, 160, 100, 255),	// stereo
	IM_COL32(180, 180, 180, 255)	// control
};

static constexpr float gridSpacing = 64.0f;
static constexpr float circuitRounding = 4.0f;

static constexpr float pinRadius = 6.0f;
static constexpr float pinBox = pinRadius * 2.0f + 2.0f;
static constexpr float topPadding = 2.0f;
static constexpr float horizontalPadding = pinRadius + 4.0f;

static constexpr float wireThickness = 1.5f;


//
//	OtAudioWidget::render
//

void OtAudioWidget::render(OtAudio* s) {
	// reset state
	audio = s;
	hoveredCircuit = 0;
	hoveredInCircuitContent = false;
	hoveredPin = 0;
	pinLocations.clear();

	// determine visual limits of all audio
	float width = 0.0f;
	float height = 0.0f;

	audio->eachCircuit([&](OtCircuit circuit) {
		width = std::max(width, circuit->x + circuit->w);
		height = std::max(height, circuit->y + circuit->h);
	});

	// start rendering
	if (focusOnEditor) {
		ImGui::SetNextWindowFocus();
		focusOnEditor = false;
	}

	ImGuiChildFlags flags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_HorizontalScrollbar |
		ImGuiWindowFlags_NoNavInputs;

	ImGui::SetNextWindowContentSize(ImVec2(width, height));
	ImGui::BeginChild("audio", ImVec2(), ImGuiChildFlags_None, flags);
	pinOffset = ImGui::GetStyle().FramePadding.y + ImGui::GetFontBaked()->Ascent - pinRadius + 1.0f;
	ImDrawList* drawlist = ImGui::GetWindowDrawList();
	widgetOffset = ImGui::GetCursorScreenPos();

	// render the grid
	if (renderingGrid) {
		renderGrid(drawlist);
	}

	// render rubber band
	renderRubberBand(drawlist);

	// render each circuit
	drawlist->ChannelsSplit(2);
	drawlist->ChannelsSetCurrent(1);

	audio->eachCircuit([&](OtCircuit circuit) {
		renderCircuit(drawlist, circuit);

		if (circuit->needsSaving) {
			circuit->needsSaving = false;
			editedCircuit = circuit->id;
			circuitEdited = true;
		}
	});

	// render each wire
	drawlist->ChannelsSetCurrent(0);

	audio->eachWire([&](OtWire& wire) {
		if (wire->id != ignoreWire) {
			renderWire(
				drawlist,
				widgetOffset + pinLocations[wire->from->id],
				widgetOffset + pinLocations[wire->to->id],
				pinColors[static_cast<size_t>(wire->from->type)]);
		}
	});

	// handle user interactions (if required)
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
		handleInteractions();

		// are we in the process of connecting audio?
		if (interactionState == InteractionState::connecting || interactionState == InteractionState::reconnecting) {
			if (outputToInput) {
				renderWire(drawlist, widgetOffset + fromPinPos, widgetOffset + toPinPos, wireColor);

			} else {
				renderWire(drawlist, widgetOffset + toPinPos, widgetOffset + fromPinPos, wireColor);
			}
		}
	}

	// we should be done rendering now
	drawlist->ChannelsMerge();
	ImGui::EndChild();
}


//
//	OtAudioWidget::isCircuitEdited
//

bool OtAudioWidget::isCircuitEdited(uint32_t& circuit) {
	if (circuitEdited) {
		circuit = editedCircuit;
		circuitEdited = false;
		return true;

	} else {
		return false;
	}
}


//
//	OtAudioWidget::isCreatingWire
//

bool OtAudioWidget::isCreatingWire(uint32_t& from, uint32_t& to) {
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
//	OtAudioWidget::isDroppingWire
//

bool OtAudioWidget::isDroppingWire(uint32_t& from, uint32_t& to) {
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
//	OtAudioWidget::isChangingWire
//

bool OtAudioWidget::isChangingWire(uint32_t& from, uint32_t& oldTo, uint32_t& newTo) {
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
//	OtAudioWidget::isDraggingComplete
//

bool OtAudioWidget::isDraggingComplete(ImVec2& offset)
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
//	OtAudioWidget::isRequestingContextMenu
//

bool OtAudioWidget::isRequestingContextMenu() {
	if (contextMenuDone) {
		contextMenuDone = false;
		return true;

	} else {
		return false;
	}
}


//
//	OtAudioWidget::renderGrid
//

void OtAudioWidget::renderGrid(ImDrawList* drawlist) {
	auto left = ImGui::GetScrollX();
	auto right = left + ImGui::GetWindowWidth();
	auto top = ImGui::GetScrollY();
	auto bottom = top + ImGui::GetWindowHeight();

	for (float x = std::floor(left / gridSpacing) * gridSpacing; x <= right; x += gridSpacing) {
		drawlist->AddLine(widgetOffset + ImVec2(x, top), widgetOffset + ImVec2(x, bottom), gridColor);
	}

	for (float y = std::floor(top / gridSpacing) * gridSpacing; y <= bottom; y += gridSpacing) {
		drawlist->AddLine(widgetOffset + ImVec2(left, y), widgetOffset + ImVec2(right, y), gridColor);
	}
}


//
//	OtAudioWidget::renderRubberBand
//

void OtAudioWidget::renderRubberBand(ImDrawList* drawlist) {
	// render rubber band
	if (interactionState == InteractionState::rubberBand) {
		drawlist->AddRectFilled(widgetOffset + rubberBandTopLeft, widgetOffset + rubberBandBottomRight, rubberBandBackgroundColor);
		drawlist->AddRect(widgetOffset + rubberBandTopLeft, widgetOffset + rubberBandBottomRight, rubberBandOutlineColor);
	}
}


//
//	OtAudioWidget::renderCircuit
//

void OtAudioWidget::renderCircuit(ImDrawList* drawlist, OtCircuit circuit) {
	// update circuit size if required
	if (circuit->needsSizing) {
		calculateCircuitSize(circuit);
		circuit->needsSizing = false;
	}

	// start the circuit rendering
	ImGui::PushID(circuit->id);
	ImVec2 topLeft = widgetOffset + ImVec2(circuit->x, circuit->y);

	// handle dragging offset
	if (interactionState == InteractionState::dragCircuits && circuit->selected) {
		topLeft += draggingOffset;
	}

	ImVec2 circuitSize = ImVec2(circuit->w, circuit->h);
	ImVec2 bottomRight = topLeft + circuitSize;
	ImVec2 headerBottomRight = ImVec2(bottomRight.x, topLeft.y + ImGui::GetFrameHeight() + topPadding);

	// render the circuit's background
	drawlist->AddRectFilled(topLeft, bottomRight, circuit->error.size() ? errorBackgroundColor : circuitBackgroundColor, circuitRounding);
	drawlist->AddRectFilled(topLeft, headerBottomRight, circuitColors[static_cast<int>(circuit->category)], circuitRounding);
	drawlist->AddRect(topLeft, bottomRight, circuit->selected ? circuitSelectedColor : circuitOutlineColor, circuitRounding);

	// is mouse over circuit?
	if (OtUi::isMouseInRect(topLeft, bottomRight)) {
		// yes, remember the ID
		hoveredCircuit = circuit->id;
		hoveredInCircuitContent = true;
	}

	// handle mouse interactions with header
	bool startCircuitRenaming = false;
	ImGui::SetCursorScreenPos(topLeft);
	ImGui::InvisibleButton("", headerBottomRight - topLeft);

	if (ImGui::IsItemHovered()) {
		// the header doesn't count as the circuit content area
		hoveredInCircuitContent = false;

		// handle double clicks (to start editing circuit title)
		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			if (renamingCircuit != circuit->id) {
				renamingCircuit = circuit->id;
				startCircuitRenaming = true;
			}
		}
	}

	// render title
	ImGui::SetCursorScreenPos(topLeft + ImVec2(horizontalPadding, topPadding));
	ImGui::BeginGroup();
	OtUi::hSpacer(((circuit->w - horizontalPadding * 2.0f) - ImGui::CalcTextSize(circuit->title.c_str()).x) / 2.0f);
	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(renamingCircuit == circuit->id ? "" : circuit->title.c_str());

	// render all output pins
	circuit->eachOutput([&](OtCircuitPin pin) {
		renderPin(drawlist, pin, bottomRight.x, circuit->w);
	});

	// do the custom circuit rendering
	circuit->customRendering(circuit->w - horizontalPadding * 2.0f);

	// render all input pins
	circuit->eachInput([&](OtCircuitPin pin) {
		renderPin(drawlist, pin, topLeft.x, circuit->w);
	});

	ImGui::EndGroup();

	if (circuit->id == renamingCircuit) {
		ImGui::SetCursorScreenPos(topLeft + ImVec2(horizontalPadding, topPadding));
		ImGui::SetNextItemWidth(circuit->w - 2.0f * horizontalPadding);

		if (startCircuitRenaming) {
			ImGui::SetKeyboardFocusHere();
			circuit->oldState = circuit->serialize().dump();
		}

		if (OtUi::inputText("##rename", &circuit->title)) {
			circuit->newState = circuit->serialize().dump();
			editedCircuit = circuit->id;
			circuitEdited = true;
			renamingCircuit = 0;

		} else if (ImGui::IsItemDeactivated()) {
			renamingCircuit = 0;
		}

		// don't know why this is required in this case
		if (ImGui::IsItemHovered()) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
		}
	}

	// render error tooltip (if required)
	if (hoveredInCircuitContent && circuit->error.size()) {
		std::string message = std::string("Error: ") + circuit->error;
		ImGui::PushStyleColor(ImGuiCol_Border, errorTooltipColor);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 3.0f);
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(message.c_str());
		ImGui::EndTooltip();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	}

	ImGui::SetCursorScreenPos(widgetOffset);
	ImGui::PopID();
}


//
//	OtAudioWidget::renderPin
//

void OtAudioWidget::renderPin(ImDrawList* drawlist, OtCircuitPin pin, float x, float w) {
	// render the pin
	auto color = pinColors[static_cast<size_t>(pin->type)];
	auto pos = ImVec2(x, ImGui::GetCursorScreenPos().y + pinOffset);
	drawlist->AddCircleFilled(pos, pinRadius, color, 12);

	// remember pin location
	pinLocations[pin->id] = pos - widgetOffset;

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
		try {
			pin->circuit->error.clear();
			pin->render(w - horizontalPadding * 2.0f);

		} catch (OtException& e) {
			pin->circuit->error = e.getShortErrorMessage();
		}

	} else {
		// right align labels for output pins
		if (pin->isOutput()) {
			OtUi::hSpacer(w - ImGui::CalcTextSize(pin->name).x - horizontalPadding * 2.0f);
		}

		// render label
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(pin->name);
	}
}


//
//	OtAudioWidget::renderWire
//

void OtAudioWidget::renderWire(ImDrawList* drawlist, const ImVec2& start, const ImVec2& end, ImU32 color) {
	// see what kind of line we need
	if (start.x < end.x) {
		// we need a simple left to right curved line
		auto distanceX = end.x - start.x;
		auto distanceY = end.y - start.y;
		auto length = std::sqrt(distanceX * distanceX + distanceY * distanceY);
		auto offset = ImVec2(0.25f * length, 0.0f);
		int segments = std::max(int(length / 4.0f), 2) | 1;
		drawlist->AddBezierCubic(start, start + offset, end - offset, end, color, wireThickness, segments);

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

		drawlist->AddBezierCubic(start, control1a, control1b, center1, color, wireThickness);
		drawlist->AddLine(center1, center2, color, wireThickness);
		drawlist->AddBezierCubic(center2, control2a, control2b, end, color, wireThickness);
	}
}


//
//	OtAudioWidget::calculateCircuitSize
//

void OtAudioWidget::calculateCircuitSize(OtCircuit circuit) {
	// determine widest line
	// first we look at circuit title width
	auto w = ImGui::CalcTextSize(circuit->title.c_str()).x;

	// then we look at each pin
	circuit->eachPin([&](OtCircuitPin pin) {
		if (pin->hasRenderer) {
			// use dimensions for custom pin rendering
			w = std::max(w, pin->renderingWidth);

		} else {
			// just use pin label width
			w = std::max(w, ImGui::CalcTextSize(pin->name).x);
		}
	});

	// take any circuit custom rendering into account
	w = std::max(w, circuit->getCustomRenderingWidth());
	circuit->w = w + horizontalPadding * 2.0f;

	// determine height
	circuit->h =
		topPadding +
		ImGui::GetFrameHeightWithSpacing() +
		ImGui::GetFrameHeightWithSpacing() * circuit->getOutputPinCount() +
		circuit->getCustomRenderingHeight() +
		ImGui::GetFrameHeightWithSpacing() * circuit->getInputPinCount();
}


//
//	OtAudioWidget::handleInteractions
//

void OtAudioWidget::handleInteractions() {
	// get mouse position relative to graph
	auto mousePos = ImGui::GetMousePos() - widgetOffset;

	// see if we have the start of a new mouse interaction
	if (interactionState == InteractionState::none) {
		// handle left mouse button events
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			// are we hitting a pin?
			if (hoveredPin) {
				auto pin = audio->getPin(hoveredPin);

				// for an output pin we handle a new connection
				if (pin->isOutput()) {
					fromPin = hoveredPin;
					fromPinPos = pinLocations[pin->id];
					toPin = 0;
					toPinPos = fromPinPos;
					outputToInput = true;
					interactionState = InteractionState::connecting;

				// for a connected input pin we handle a (re/dis)connect
				} else if (pin->isSourceConnected()) {
					fromPin = pin->getSource()->id;
					fromPinPos = pinLocations[fromPin];
					oldToPin = pin->id;
					toPin = 0;
					toPinPos = pinLocations[pin->id];
					ignoreWire = audio->findWire(fromPin, oldToPin)->id;
					outputToInput = true;
					interactionState = InteractionState::reconnecting;

				// for an unconnected input pin we handle a new connection
				} else {
					fromPin = hoveredPin;
					fromPinPos = pinLocations[pin->id];
					toPin = 0;
					toPinPos = fromPinPos;
					outputToInput = false;
					interactionState = InteractionState::connecting;
				}

			// are we hitting a circuit?
			} else if (hoveredCircuit) {
				// yes, is it the content area (body)?
				if (hoveredInCircuitContent) {
					// ignore mouse events until mouse is released
						interactionState = InteractionState::ignoreMouse;

				} else {
					// we hit the header, handle circuit selection
					if (!audio->isCircuitSelected(hoveredCircuit)) {
						audio->select(hoveredCircuit, !ImGui::IsKeyDown(ImGuiMod_Shift));
					}

					interactionState = InteractionState::selectCircuit;
				}

			// are we hitting the background?
			} else if (ImGui::IsWindowHovered()) {
				// start rubber banding
				rubberBandStartPos = mousePos;
				rubberBandTopLeft = rubberBandStartPos;
				rubberBandBottomRight = rubberBandStartPos;
				interactionState = InteractionState::rubberBand;
			}

		// handle middle mouse button events (panning)
		} else if (ImGui::IsMouseDown(ImGuiMouseButton_Middle) && ImGui::IsWindowHovered()) {
			ImVec2 mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
			ImGui::SetScrollX(ImGui::GetScrollX() - mouseDelta.x);
			ImGui::SetScrollY(ImGui::GetScrollY() - mouseDelta.y);
			ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);

		// handle right mouse button events (context menu)
		} else if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
			if (!hoveredPin && !hoveredCircuit) {
				contextMenuDone = true;
				contextMenuPos = mousePos;
			}
		}

	// handle ignore mouse state
	} else if (interactionState == InteractionState::ignoreMouse) {
		if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			interactionState = InteractionState::none;
		}

	// handle select circuit state
	} else if (interactionState == InteractionState::selectCircuit) {
		// end state if required
		if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			interactionState = InteractionState::none;

		// see if we started dragging
		} else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
			draggingStartPos = mousePos;
			interactionState = InteractionState::dragCircuits;
		}

	// handle drag circuit state
	} else if (interactionState == InteractionState::dragCircuits) {
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			draggingOffset = mousePos - draggingStartPos;

		} else {
			draggingDone = true;
			draggedOffset = draggingOffset;
			draggingOffset = ImVec2();
			interactionState = InteractionState::none;
		}

	// handle rubber band state
	} else if (interactionState == InteractionState::rubberBand) {
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			rubberBandTopLeft = ImVec2(
				std::min(mousePos.x, rubberBandStartPos.x),
				std::min(mousePos.y, rubberBandStartPos.y));

			rubberBandBottomRight = ImVec2(
				std::max(mousePos.x, rubberBandStartPos.x),
				std::max(mousePos.y, rubberBandStartPos.y));

			audio->select(
				static_cast<int>(rubberBandTopLeft.x),
				static_cast<int>(rubberBandTopLeft.y),
				static_cast<int>(rubberBandBottomRight.x),
				static_cast<int>(rubberBandBottomRight.y));

		} else {
			interactionState = InteractionState::none;
		}

	// handle circuit connection (create a new wire)
	} else if (interactionState == InteractionState::connecting) {
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			// are we hovering over another pin?
			if (hoveredPin && hoveredPin != fromPin) {
				toPin = hoveredPin;
				toPinPos = pinLocations[hoveredPin];

				if (outputToInput) {
					wireValid = audio->isWireValid(fromPin, toPin);

				} else {
					wireValid = audio->isWireValid(toPin, fromPin);
				}

				wireColor = wireValid ? validWireColor : invalidWireColor;

			} else {
				toPin = 0;
				toPinPos = mousePos;
				wireValid = false;
				wireColor = creatingWireColor;
			}

		} else {
			if (wireValid) {
				connectingDone = true;
			}

			interactionState = InteractionState::none;
		}

	// handle circuit reconnection (change an existing wire)
	} else if (interactionState == InteractionState::reconnecting) {
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			// are we hovering over another pin?
			if (hoveredPin) {
				toPin = hoveredPin;
				toPinPos = pinLocations[hoveredPin];

				if (toPin == oldToPin) {
					wireValid = true;

				} else {
					wireValid = audio->isWireValid(fromPin, toPin);
				}

				wireColor = wireValid ? validWireColor : invalidWireColor;

			} else {
				toPin = 0;
				toPinPos = mousePos;
				wireValid = false;
				wireColor = creatingWireColor;
			}

		} else {
			if (wireValid) {
				if (toPin != oldToPin) {
					reconnectingDone = true;
				}

			} else {
				disconnectingDone = true;
			}

			ignoreWire = 0;
			interactionState = InteractionState::none;
		}
	}
}
