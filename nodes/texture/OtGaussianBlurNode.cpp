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
	inline void onFilter(OtTexture& input, OtFrameBuffer& output) override {
		temp1.initialize(output.getColorTextureType());
		temp1.update(output.getWidth(), output.getHeight());

		temp2.initialize(output.getColorTextureType());
		temp2.update(output.getWidth(), output.getHeight());

		for (auto i = 0; i < iterations; i++) {
			// run horizontal blur
			gaussian.setDirection(glm::vec2(1.0f, 0.0f));

			if (i == 0) {
				gaussian.render(input, temp1);

			} else {
				gaussian.render(temp2, temp1);

			}

			// run vertical blur
			gaussian.setRadius(radius);
			gaussian.setDirection(glm::vec2(0.0f, 1.0f));

			if (i == iterations - 1) {
				gaussian.render(temp1, output);

			} else {
				gaussian.render(temp1, temp2);

			}
		}
	}

	static constexpr const char* nodeName = "Gaussian Blur";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::texture;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

	// properties
	float radius = 2.0f;
	int iterations = 2;
	OtGaussian gaussian;
	OtFrameBuffer temp1{};
	OtFrameBuffer temp2;
};

static OtNodesFactoryRegister<OtGaussianBlurNode> registration;
