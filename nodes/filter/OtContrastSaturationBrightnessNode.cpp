//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtContrastSaturationBrightness.h"

#include "OtNodesFactory.h"
#include "OtTextureFilterNode.h"


//
//	OtContrastSaturationBrightnessNode
//

class OtContrastSaturationBrightnessNode : public OtTextureFilterNode {
public:
	// configure node
	inline void configure() override {
		OtTextureFilterNode::configure();
		addInputPin("Contrast", contrast);
		addInputPin("Saturation", saturation);
		addInputPin("Brightness", brightness);
	}

	// run filter
	void onFilter(OtTexture& input, OtFrameBuffer& output) override {
		csb.setContrast(contrast);
		csb.setSaturation(saturation);
		csb.setBrightness(brightness);
		csb.render(input, output);
	}

	static constexpr const char* nodeName = "Contrast Saturation Brightness";
	static constexpr int nodeCategory = OtNodeClass::filter;
	static constexpr int nodeKind = OtNodeClass::fixed;

	// properties
	OtContrastSaturationBrightness csb;
	float contrast = 1.0f;
	float saturation = 1.0f;
	float brightness = 1.0f;
};

static OtNodesFactoryRegister<OtContrastSaturationBrightnessNode> type;