//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtCheckerBoard.h"
#include "OtColor.h"

#include "OtNodesFactory.h"
#include "OtTextureGeneratorNode.h"


//
//	OtCheckerBoardGeneratorNode
//

class OtCheckerBoardGeneratorNode : public OtTextureGeneratorNode {
public:
	// configure node
	inline void configure() override {
		OtTextureGeneratorNode::configure();
		addInputPin("Repeat", repeat);
		addInputPin("Black Color", blackColor);
		addInputPin("White Color", whiteColor);
	}

	// run the texture generator
	void onGenerate(OtTexture& output) override {
		checkerboard.setRepeat(repeat);
		checkerboard.setBlackColor(blackColor);
		checkerboard.setWhiteColor(whiteColor);
		checkerboard.render(output);
	}

	static constexpr const char* nodeName = "CheckerBoard Generator";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::texture;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	float repeat = 1.0f;
	OtColor blackColor{0.0f, 0.0f, 0.0f};
	OtColor whiteColor{1.0f, 1.0f, 1.0f};
	OtCheckerBoard checkerboard;
};

static OtNodesFactoryRegister<OtCheckerBoardGeneratorNode> registration;
