//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtColorWheel.h"

#include "OtNodesFactory.h"
#include "OtTextureGeneratorNode.h"


//
//	OtColorWheelGeneratorNode
//

class OtColorWheelGeneratorNode : public OtTextureGeneratorNode {
public:
	// run the texture generator
	void onGenerate(OtFrameBuffer& output) override {
		colorwheel.render(output);
	}

	static constexpr const char* nodeName = "ColorWheel Generator";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::generate;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	OtColorWheel colorwheel;
};

static OtNodesFactoryRegister<OtColorWheelGeneratorNode> registration;
