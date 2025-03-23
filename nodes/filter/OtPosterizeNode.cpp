//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtPosterize.h"

#include "OtNodesFactory.h"
#include "OtTextureFilterNode.h"


//
//	OtPosterizeNode
//

class OtPosterizeNode : public OtTextureFilterNode {
public:
	// configure node
	inline void configure() override {
		OtTextureFilterNode::configure();
		addInputPin("Levels", levels);
	}

	// run filter
	inline void onFilter(OtTexture& input, OtFrameBuffer& output) override {
		posterizer.setLevels(levels);
		posterizer.render(input, output);
	}

	static constexpr const char* nodeName = "Texture Posterizer";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::filter;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

	// properties
	OtPosterize posterizer;
	int levels = 10;
};

static OtNodesFactoryRegister<OtPosterizeNode> registration;
