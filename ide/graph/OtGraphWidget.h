//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>

#include "imgui.h"

#include "OtGraph.h"
#include "OtGraphNode.h"


//
//	OtGraphWidget
//

class OtGraphWidget {
public:
	// render widget
	void render(OtGraph* graph);

	// check for interactions
	bool isNodeEdited(uint32_t& node);
	bool isCreatingLink(uint32_t& from, uint32_t& to);
	bool isDroppingLink(uint32_t& from, uint32_t& to);
	bool isChangingLink(uint32_t& from, uint32_t& oldTo, uint32_t& newTo);
	bool isDraggingComplete(ImVec2& offset);
	bool showContextMenu();

	// convert coordinates from screen to widget space
	ImVec2 screenToWidget(const ImVec2& in);

private:
	// render the parts
	void renderGrid(ImDrawList* drawlist);
	void renderRubberBand(ImDrawList* drawlist);
	void renderNode(ImDrawList* drawlist, OtGraphNode& node);
	void renderLink(ImDrawList* drawlist, const ImVec2& start, const ImVec2& end, ImU32 color);
	void renderPin(ImDrawList* drawlist, OtGraphPin& pin, float x, float w);

	// utility functions
	void calculateNodeSize(OtGraphNode& node);
	void handleInteractions(ImDrawList* drawlist);

	// target graph
	OtGraph* graph;

	// positional properties
	ImVec2 position;					// in screen space
	ImVec2 size;
	ImVec2 offset;						// between screen and widget space
	float pinOffset;

	// pin locations in screen space
	std::unordered_map<uint32_t, ImVec2> pinLocations;

	// interaction properties
	enum {
		noInteraction,
		selectNode,
		dragNodes,
		rubberBand,
		scrolling,
		connecting,
		reconnecting
	};

	int interactionState = noInteraction;

	bool nodeEdited = false;
	bool draggingDone = false;
	bool connectingDone = false;
	bool disconnectingDone = false;
	bool reconnectingDone = false;
	bool contextMenuDone = false;

	uint32_t editedNode;
	uint32_t hoveredNode;
	uint32_t hoveredPin;

	ImVec2 rubberBandStartPos;			// in screen space
	ImVec2 rubberBandTopLeft;			// in screen space
	ImVec2 rubberBandBottomRight;		// in screen space

	ImVec2 draggingStartPos;			// in screen space
	ImVec2 draggingOffset;
	ImVec2 draggedOffset;

	ImVec2 scrollingStartPos;			// in screen space
	ImVec2 scrollingOffset{0.0f, 0.0f};

	uint32_t ignoreLink = 0;
	uint32_t fromPin = 0;
	uint32_t toPin = 0;
	uint32_t oldToPin = 0;
	ImVec2 fromPinPos;					// in screen space
	ImVec2 toPinPos;					// in screen space
	bool linkValid;
	ImU32 linkColor;
};
