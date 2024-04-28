//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <string>

#include "OtFont.h"
#include "OtShape.h"

#include "OtNodesFactory.h"


//
//	OtTextToShapeNode
//

class OtTextToShapeNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Font", font);
		addInputPin("Text", text);
		addInputPin("Size", size);
		addInputPin("Center", center);
		addOutputPin("Shape", shape);
	}

	// convert text to a 2D shape
	void onExecute() override {
		shape.clear();

		if (font.isValid() && text.size() && size != 0.0f) {
			shape.text(font, text, size, center);
		}
	}

	static constexpr const char* nodeName = "Text to Shape";
	static constexpr int nodeCategory = OtNodeClass::shape;
	static constexpr int nodeKind = OtNodeClass::fixed;

protected:
	OtFont font;
	std::string text;
	float size = 12.0f;
	bool center = true;
	OtShape shape;
};

static OtNodesFactoryRegister<OtTextToShapeNode> type;
