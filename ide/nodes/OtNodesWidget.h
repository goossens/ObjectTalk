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

#include "OtNodes.h"
#include "OtNode.h"


//
//	OtNodesWidget
//

class OtNodesWidget {
public:
	// render widget
	void render(OtNodes* nodes);

	// access render options
	inline void setGridRendering(bool flag) { renderingGrid = flag; }
	inline bool isRenderingGrid() { return renderingGrid; }

	// check for interactions
	bool isNodeEdited(uint32_t& node);
	bool isCreatingLink(uint32_t& from, uint32_t& to);
	bool isDroppingLink(uint32_t& from, uint32_t& to);
	bool isChangingLink(uint32_t& from, uint32_t& oldTo, uint32_t& newTo);
	bool isDraggingComplete(ImVec2& offset);
	bool isRequestingContextMenu();
	inline ImVec2 getContextMenuPos() { return contextMenuPos; }

private:
	// render the parts
	void renderGrid(ImDrawList* drawlist);
	void renderRubberBand(ImDrawList* drawlist);
	void renderNode(ImDrawList* drawlist, OtNode& node);
	void renderLink(ImDrawList* drawlist, const ImVec2& start, const ImVec2& end, ImU32 color, bool isVarying=false);
	void renderPin(ImDrawList* drawlist, OtNodesPin& pin, float x, float w);

	// utility functions
	void calculateNodeSize(OtNode& node);
	void handleInteractions();

	// target nodes
	OtNodes* nodes;

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
		selectNode,
		dragNodes,
		rubberBand,
		connecting,
		reconnecting
	};

	InteractionState interactionState = InteractionState::none;

	bool nodeEdited = false;
	bool draggingDone = false;
	bool connectingDone = false;
	bool disconnectingDone = false;
	bool reconnectingDone = false;
	bool contextMenuDone = false;

	uint32_t editedNode;
	uint32_t hoveredNode;
	bool hoveredInNodeContent;
	uint32_t renamingNode;
	uint32_t hoveredPin;

	ImVec2 rubberBandStartPos;
	ImVec2 rubberBandTopLeft;
	ImVec2 rubberBandBottomRight;

	ImVec2 contextMenuPos;

	ImVec2 draggingStartPos;
	ImVec2 draggingOffset;
	ImVec2 draggedOffset;

	uint32_t ignoreLink = 0;
	uint32_t fromPin = 0;
	uint32_t toPin = 0;
	uint32_t oldToPin = 0;
	ImVec2 fromPinPos;
	ImVec2 toPinPos;
	bool outputToInput;
	bool linkValid;
	ImU32 linkColor;

	bool focusOnEditor = true;
};
