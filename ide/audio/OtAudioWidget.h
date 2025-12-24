//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>

#include "imgui.h"

#include "OtAudio.h"
#include "OtCircuit.h"


//
//	OtAudioWidget
//

class OtAudioWidget {
public:
	// render widget
	void render(OtAudio* audio);

	// access render options
	inline void setGridRendering(bool flag) { renderingGrid = flag; }
	inline bool isRenderingGrid() { return renderingGrid; }

	// check for interactions
	bool isCircuitEdited(uint32_t& circuit);
	bool isCreatingWire(uint32_t& from, uint32_t& to);
	bool isDroppingWire(uint32_t& from, uint32_t& to);
	bool isChangingWire(uint32_t& from, uint32_t& oldTo, uint32_t& newTo);
	bool isDraggingComplete(ImVec2& offset);
	bool isRequestingContextMenu();
	inline ImVec2 getContextMenuPos() { return contextMenuPos; }

private:
	// render the parts
	void renderGrid(ImDrawList* drawlist);
	void renderRubberBand(ImDrawList* drawlist);
	void renderCircuit(ImDrawList* drawlist, OtCircuit circuit);
	void renderWire(ImDrawList* drawlist, const ImVec2& start, const ImVec2& end, ImU32 color);
	void renderPin(ImDrawList* drawlist, OtCircuitPin pin, float x, float w);

	// utility functions
	void calculateCircuitSize(OtCircuit circuit);
	void handleInteractions();

	// target audio
	OtAudio* audio;

	// rendering options
	bool renderingGrid = true;

	// positional properties
	ImVec2 widgetOffset;
	float pinOffset;

	// pin locations in screen space
	std::unordered_map<uint32_t, ImVec2> pinLocations;

	// interaction properties
	enum class InteractionState {
		none,
		ignoreMouse,
		selectCircuit,
		dragCircuits,
		rubberBand,
		connecting,
		reconnecting
	};

	InteractionState interactionState = InteractionState::none;

	bool circuitEdited = false;
	bool draggingDone = false;
	bool connectingDone = false;
	bool disconnectingDone = false;
	bool reconnectingDone = false;
	bool contextMenuDone = false;

	uint32_t editedCircuit;
	uint32_t hoveredCircuit;
	bool hoveredInCircuitContent;
	uint32_t renamingCircuit;
	uint32_t hoveredPin;

	ImVec2 rubberBandStartPos;
	ImVec2 rubberBandTopLeft;
	ImVec2 rubberBandBottomRight;

	ImVec2 contextMenuPos;

	ImVec2 draggingStartPos;
	ImVec2 draggingOffset;
	ImVec2 draggedOffset;

	uint32_t ignoreWire = 0;
	uint32_t fromPin = 0;
	uint32_t toPin = 0;
	uint32_t oldToPin = 0;
	ImVec2 fromPinPos;
	ImVec2 toPinPos;
	bool outputToInput;
	bool wireValid;
	ImU32 wireColor;

	bool focusOnEditor = true;
};
