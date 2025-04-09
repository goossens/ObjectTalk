//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtFrameBuffer.h"
#include "OtMap.h"

#include "OtNodesFactory.h"


//
//	OtMapToHeightMapNode
//

class OtMapToHeightMapNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Map", map);
		addInputPin("Size", size);
		addOutputPin("Texture", texture);
	}

	// validate input parameters
	inline void onValidate() override {
		size = std::clamp(size, 32, 1024);
	}

	// generate the heightmap
	inline void onExecute() override {
		if (map.isValid()) {
			map.renderHeightMap(framebuffer, size);
			texture = framebuffer.getColorTexture();
			texture.setVersion(version++);

		} else {
			texture.clear();
		}
	}

	static constexpr const char* nodeName = "Map to HeightMap";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::map;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	int size = 32;

	// map component
	OtMap map;
	OtFrameBuffer framebuffer{OtTexture::r8Texture};
	OtTexture texture;
	int version = 1;
};


static OtNodesFactoryRegister<OtMapToHeightMapNode> registration;
