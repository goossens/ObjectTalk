//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtSharpen.h"

#include "OtNodesFactory.h"
#include "OtTextureFilterNode.h"


//
//	OtSharpenNode
//

class OtSharpenNode : public OtTextureFilterNode {
public:
	// configure node
	inline void configure() override {
		OtTextureFilterNode::configure();
		addInputPin("Strength", strength);
	}

	// run filter
	void onFilter(OtTexture& input, OtFrameBuffer& output) override {
		sharpener.setStrength(strength);
		sharpener.render(input, output);
	}

	static constexpr const char* nodeName = "Texture Sharpener";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::filter;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

	// properties
	OtSharpen sharpener;
	float strength = 1.0f;
};

static OtNodesFactoryRegister<OtSharpenNode> type;
