//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::filter;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

	// properties
	OtSeamlessTile tiler;
};

static OtNodesFactoryRegister<OtSeamlessTilerNode> type;
