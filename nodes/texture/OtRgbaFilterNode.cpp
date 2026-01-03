//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtRgbaAdjust.h"

#include "OtNodesFactory.h"
#include "OtTextureFilterNode.h"


//
//	OtRgbaNode
//

class OtRgbaNode : public OtTextureFilterNode {
public:
	// configure node
	inline void configure() override {
		OtTextureFilterNode::configure();
		addInputPin("Red", red);
		addInputPin("Green", green);
		addInputPin("Blue", blue);
		addInputPin("Alpha", alpha);
	}

	// validate input
	inline void onValidate() override {
		red = std::clamp(red, 0.0f, 1.0f);
		green = std::clamp(green, 0.0f, 1.0f);
		blue = std::clamp(blue, 0.0f, 1.0f);
		alpha = std::clamp(alpha, 0.0f, 1.0f);
	}

	// run filter
	inline void onFilter(OtTexture& input, OtTexture& output) override {
		rgba.setRgba(glm::vec4(red, green, blue, alpha));
		rgba.render(input, output);
	}

	static constexpr const char* nodeName = "RGBA Filter";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::texture;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtRgbaAdjust rgba;
	float red = 1.0f;
	float green = 1.0f;
	float blue = 1.0f;
	float alpha = 1.0f;
};

static OtNodesFactoryRegister<OtRgbaNode> registration;
