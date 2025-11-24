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
#include "OtBlitPass.h"
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

		if (textures.size() == 0) {
			output.clear();

		} else {
			output.update(
				textures[0].getWidth(),
				textures[0].getHeight(),
				textures[0].getFormat(),
				textures[0].getUsage()
			);

			OtBlitPass::blit(textures[0], output);

			for (size_t i = 1; i < textures.size(); i++) {
				alphaover.render(textures[i], output);
			}

			output.setVersion(version++);
		}
	}

	static constexpr const char* nodeName = "Composite Textures";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::texture;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
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
	OtCompositingAlphaOver alphaover;
	int version = 1;
};

static OtNodesFactoryRegister<OtCompositeTexturesNode> registration;
