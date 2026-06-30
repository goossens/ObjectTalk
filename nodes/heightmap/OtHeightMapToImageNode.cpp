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
//	OtHeightMapToImageNode
//

class OtHeightMapToImageNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", heightMap);
		addOutputPin("Output", image);
	}

	// when the input changes, we write the heightMap to the GPUs image
	inline void onExecute() override {
		if (heightMap.isValid()) {
			heightMap.saveToImage(image);

		} else {
			image.clear();
		}
	}

	static constexpr const char* nodeName = "Height Map to Image";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::heightmap;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:œ
	// properties
	OtHeightMap heightMap;
	OtImage image;
};

static OtNodesFactoryRegister<OtHeightMapToImageNode> registration;
