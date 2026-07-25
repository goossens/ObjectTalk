//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtImage.h"

#include "OtHeightMap.h"

#include "OtNodesFactory.h"


//
//	OtImageToHeightMapNode
//

class OtImageToHeightMapNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", image);
		addOutputPin("Output", heightMap);
	}

	// when the input changes, we write the image to the GPUs heightMap
	inline void onExecute() override {
		if (image.isValid()) {
			heightMap.loadFromImage(image);

		} else {
			heightMap.clear();
		}
	}

	static constexpr const char* nodeName = "Image to Height Map";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::image;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtImage image;
	OtHeightMap heightMap;
};

static OtNodesFactoryRegister<OtImageToHeightMapNode> registration;
