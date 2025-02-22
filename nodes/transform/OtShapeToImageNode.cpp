//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "imgui.h"

#include "OtColor.h"
#include "OtImage.h"
#include "OtShape.h"

#include "OtNodesFactory.h"


//
//	OtShapeToImageNode
//

class OtShapeToImageNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Shape", shape);
		addInputPin("Color", color);
		addInputPin("Outline", outline);
		addOutputPin("Image", image);
	}

	// convert image to texture and determine size
	void onExecute() override {
		if (shape.isValid()) {
			if (outline) {
				shape.renderStroke(image, color, 2.0f);

			} else {
				shape.renderFill(image, color);
			}

			image.incrementVersion();

		} else {
			image.clear();
		}
	}

	static constexpr const char* nodeName = "Shape to Image";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::transform;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	OtShape shape;
	OtColor color{1.0f, 0.0f, 0.0f, 1.0f};
	bool outline = false;
	OtImage image;
};

static OtNodesFactoryRegister<OtShapeToImageNode> registration;
