//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtSeamlessTile.h"

#include "OtNodesFactory.h"
#include "OtTextureFilterNode.h"


//
//	OtSeamlessTilerNode
//

class OtSeamlessTilerNode : public OtTextureFilterNode {
public:
	// constructor
	inline OtSeamlessTilerNode() : OtTextureFilterNode(name) {}

	// run filter
	void onFilter() override {
		tiler.render(inputTexture, framebuffer);
	}

	static constexpr const char* name = "Seamless Tiler";

	// properties
	OtSeamlessTile tiler;
};

static OtNodesFactoryRegister<OtSeamlessTilerNode> type("Filters");
