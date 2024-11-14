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
	// run filter
	void onFilter(OtTexture& input, OtFrameBuffer& output) override {
		tiler.render(input, output);
	}

	static constexpr const char* nodeName = "Seamless Tiler";
	static constexpr int nodeCategory = OtNodeClass::filter;
	static constexpr int nodeKind = OtNodeClass::fixed;

	// properties
	OtSeamlessTile tiler;
};

static OtNodesFactoryRegister<OtSeamlessTilerNode> type;
