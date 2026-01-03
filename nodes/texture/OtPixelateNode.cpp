//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtPixelate.h"

#include "OtNodesFactory.h"
#include "OtTextureFilterNode.h"


//
//	OtPixelateNode
//

class OtPixelateNode : public OtTextureFilterNode {
public:
	// configure node
	inline void configure() override {
		OtTextureFilterNode::configure();
		addInputPin("Size", size);
	}

	// run filter
	inline void onFilter(OtTexture& input, OtTexture& output) override {
		pixelator.setSize(size);
		pixelator.render(input, output);
	}

	static constexpr const char* nodeName = "Texture Pixelator";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::texture;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtPixelate pixelator;
	int size = 5;
};

static OtNodesFactoryRegister<OtPixelateNode> registration;
