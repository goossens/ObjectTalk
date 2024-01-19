//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtGraphNode.h"
#include "OtOutputNodes.h"


//
//	OtOutputNodesRegister
//

#define REGISTER(CLASS) \
	graph.registerNodeType<CLASS>("Output", CLASS::name)

void OtOutputNodesRegister(OtGraph& graph) {
//	REGISTER(OtGraphNodeFloatOutput);
}
