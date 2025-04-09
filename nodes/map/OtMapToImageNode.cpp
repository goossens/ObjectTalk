//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtImage.h"
#include "OtMap.h"

#include "OtNodesFactory.h"


//
//	OtMapToImageNode
//

class OtMapToImageNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Map", map);
		addInputPin("Size", size);
		addInputPin("Biome", biome);
		addOutputPin("Image", image);
	}

	// validate input parameters
	inline void onValidate() override {
		size = std::clamp(size, 32, 1024);
	}

	// generate the image
	inline void onExecute() override {
		if (map.isValid()) {
			map.render(image, size, biome);

		} else {
			image.clear();
		}
	}

	static constexpr const char* nodeName = "Map to Image";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::map;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	int size = 32;
	bool biome = false;

	// map component
	OtMap map;
	OtImage image;
};


static OtNodesFactoryRegister<OtMapToImageNode> registration;
