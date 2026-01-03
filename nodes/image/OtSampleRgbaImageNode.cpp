//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtNodesFactory.h"


//
//	OtSampleRgbaImageNode
//

class OtSampleRgbaImageNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Image", image);
		addInputPin("U", u);
		addInputPin("V", v);
		addOutputPin("R", r);
		addOutputPin("G", g);
		addOutputPin("B", b);
		addOutputPin("A", a);
	}

	// combine values
	inline void onExecute() override {
		if (image.isValid()) {
			auto sample = image.sampleValueRgba(u, v);
			r = sample.r;
			g = sample.g;
			b = sample.b;
			a = sample.a;

		} else {
			r = 0.0f;
			g = 0.0f;
			b = 0.0f;
			a = 0.0f;
		}
	}

	static constexpr const char* nodeName = "Sample RGBA Image";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::image;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::flexible;

private:
	// properties
	OtImage image;
	float u{0.0f};
	float v{0.0f};
	float r{0.0f};
	float g{0.0f};
	float b{0.0f};
	float a{0.0f};
};

static OtNodesFactoryRegister<OtSampleRgbaImageNode> registration;
