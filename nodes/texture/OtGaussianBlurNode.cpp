//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtGaussian.h"

#include "OtNodesFactory.h"
#include "OtTextureFilterNode.h"


//
//	OtGaussianBlurNode
//

class OtGaussianBlurNode : public OtTextureFilterNode {
public:
	// configure node
	inline void configure() override {
		OtTextureFilterNode::configure();
		addInputPin("Radius", radius);
		addInputPin("Iterations", iterations);
	}

	// validate input
	inline void onValidate() override {
		radius = std::clamp(radius, 0.5f, 10.0f);
		iterations = std::clamp(iterations, 1, 50);
	}

	// run filter
	inline void onFilter(OtTexture& input, OtTexture& output) override {
		temp1.update(input.getWidth(), input.getHeight(), input.getFormat(), input.getUsage());
		temp2.update(input.getWidth(), input.getHeight(), input.getFormat(), input.getUsage());
		gaussian.setRadius(radius);

		for (auto i = 0; i < iterations; i++) {
			// run horizontal blur
			gaussian.setDirection(glm::vec2(1.0f, 0.0f));
			gaussian.render(i == 0 ? input : temp2, temp1);

			// run vertical blur
			gaussian.setDirection(glm::vec2(0.0f, 1.0f));
			gaussian.render(temp1, (i == iterations - 1) ? output : temp2);
		}
	}

	static constexpr const char* nodeName = "Gaussian Blur";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::texture;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	float radius = 2.0f;
	int iterations = 2;
	OtGaussian gaussian;
	OtTexture temp1;
	OtTexture temp2;
};

static OtNodesFactoryRegister<OtGaussianBlurNode> registration;
