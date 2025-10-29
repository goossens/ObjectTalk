//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <vector>
#include <utility>

#include "OtCompositingAlphaOver.h"
#include "OtFrameBuffer.h"
#include "OtTexture.h"

#include "OtNodesFactory.h"


//
//	OtCompositeTexturesNode
//

class OtCompositeTexturesNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input 1", input1);
		addInputPin("Input 2", input2);
		addInputPin("Input 3", input3);
		addInputPin("Input 4", input4);
		addInputPin("Input 5", input5);
		addInputPin("Input 6", input6);
		addInputPin("Input 7", input7);
		addInputPin("Input 8", input8);
		addOutputPin("Output", output);
	}

	// composite textures
	inline void onExecute() override {
		// build list of valid textures
		std::vector<OtTexture> textures;

		if (input1.isValid()) { textures.emplace_back(input1); }
		if (input2.isValid()) { textures.emplace_back(input2); }
		if (input3.isValid()) { textures.emplace_back(input3); }
		if (input4.isValid()) { textures.emplace_back(input4); }
		if (input5.isValid()) { textures.emplace_back(input5); }
		if (input6.isValid()) { textures.emplace_back(input6); }
		if (input7.isValid()) { textures.emplace_back(input7); }
		if (input8.isValid()) { textures.emplace_back(input8); }
		auto count = textures.size();

		if (count == 0) {
			output.clear();

		} else {
			framebuffer.initialize(textures[0].getFormat(), OtTexture::noTexture, 1, true);
			framebuffer.update(textures[0].getWidth(), textures[0].getHeight());
			auto target = framebuffer.getColorTexture();

			OtPass pass;
			pass.blit(target, textures[0]);

			for (size_t i = 1; i < textures.size(); i++) {
				alphaover.render(textures[i], framebuffer);
			}

			output = framebuffer.getColorTexture();
			output.setVersion(version++);



			// framebuffer.initialize(textures[0].getFormat());
			// framebuffer.update(textures[0].getWidth(), textures[0].getHeight());

			// if (count == 1) {
			// 	blit.render(textures[0], framebuffer);

			// } else if (count == 2) {
			// 	alphaover.setOverlay(textures[1]);
			// 	alphaover.render(textures[0], framebuffer);

			// } else {
			// 	temp1.initialize(textures[0].getFormat());
			// 	temp1.update(textures[0].getWidth(), textures[0].getHeight());

			// 	temp2.initialize(textures[0].getFormat());
			// 	temp2.update(textures[0].getWidth(), textures[0].getHeight());

			// 	for (size_t i = 0; i < count - 1; i++) {
			// 		alphaover.setOverlay(textures[i + 1]);

			// 		if (i == 0) {
			// 			alphaover.render(textures[0], temp1);

			// 		} else if (i == count - 2) {
			// 			alphaover.render(temp1, framebuffer);

			// 		} else {
			// 			alphaover.render(temp1, temp2);
			// 			std::swap(temp1, temp2);
			// 		}
			// 	}
			// }

			// output = framebuffer.getColorTexture();
			// output.setVersion(version++);
		}
	}

	static constexpr const char* nodeName = "Composite Textures";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::texture;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	// properties
	OtTexture input1;
	OtTexture input2;
	OtTexture input3;
	OtTexture input4;
	OtTexture input5;
	OtTexture input6;
	OtTexture input7;
	OtTexture input8;
	OtTexture output;

	// processing resources
	OtFrameBuffer framebuffer;
	OtCompositingAlphaOver alphaover;
	int version = 1;
};

static OtNodesFactoryRegister<OtCompositeTexturesNode> registration;
