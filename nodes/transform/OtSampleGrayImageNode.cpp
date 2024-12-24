//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtNodesFactory.h"


//
//	OtSampleGrayImageNode
//

class OtSampleGrayImageNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Image", image);
		addInputPin("U", u);
		addInputPin("V", v);
		addOutputPin("Gray", gray);
	}

	// combine values
	void onExecute() override {
		if (image.isValid()) {
			gray = image.sampleValueGray(u, v);

		} else {
			gray = 0.0f;
		}
	}

	static constexpr const char* nodeName = "Sample Gray Image";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::transform;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::flexible;

protected:
	OtImage image;
	float u{0.0f};
	float v{0.0f};
	float gray{0.0f};
};

static OtNodesFactoryRegister<OtSampleGrayImageNode> type;
