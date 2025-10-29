//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtCompositingAlphaOver.h"
#include "OtPass.h"

#include "OtNodesFactory.h"
#include "OtTextureFilterNode.h"


//
//	OtAlphaOverNode
//

class OtAlphaOverNode : public OtTextureFilterNode {
public:
	// configure node
	inline void configure() override {
		OtTextureFilterNode::configure();
		addInputPin("Overlay", overlayTexture);
	}

	// run filter
	inline void onFilter(OtTexture& input, OtFrameBuffer& output) override {
		OtPass pass;
		pass.blit(output.getColorTextureHandle(), 0, 0, input.getHandle());

		if (overlayTexture.isValid()) {
			alphaOver.render(overlayTexture, output);
		}
	}

	static constexpr const char* nodeName = "Alpha Over";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::texture;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

	// properties
	OtCompositingAlphaOver alphaOver;
	OtTexture overlayTexture;
};

static OtNodesFactoryRegister<OtAlphaOverNode> registration;
