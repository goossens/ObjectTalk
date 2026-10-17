//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtSmartDeNoise.h"

#include "OtNodesFactory.h"
#include "OtTextureFilterNode.h"


//
//	OtSmartDeNoiseNode
//

class OtSmartDeNoiseNode : public OtTextureFilterNode {
public:
	// configure node
	inline void configure() override {
		OtTextureFilterNode::configure();
		addInputPin("Sigma", sigma);
		addInputPin("kSigma", kSigma);
		addInputPin("Threshold", threshold);
	}

	// run filter
	inline void onFilter(OtTexture& input, OtTexture& output) override {
		// limit values
		sigma = std::clamp(sigma, 0.01f, 20.0f);
		kSigma = std::clamp(kSigma, 0.01f, 3.0f);
		threshold = std::clamp(threshold, 0.01f, 1.0f);

		denoise.setSigma(sigma);
		denoise.setkSigma(kSigma);
		denoise.setThreshold(threshold);
		denoise.render(input, output);
	}

	static constexpr const char* nodeName = "Smart DeNoise";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::texture;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	float sigma = 7.000f;
	float kSigma = 3.0f;
	float threshold = 0.195f;
	OtSmartDeNoise denoise;
};

static OtNodesFactoryRegister<OtSmartDeNoiseNode> registration;
