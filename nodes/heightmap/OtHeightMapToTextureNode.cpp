//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtTexture.h"

#include "OtHeightMap.h"

#include "OtNodesFactory.h"


//
//	OtHeightMapToTextureNode
//

class OtHeightMapToTextureNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", heightMap);
		addOutputPin("Output", texture);
	}

	// when the input changes, we write the heightMap to the GPUs texture
	inline void onExecute() override {
		if (heightMap.isValid()) {
			heightMap.saveToTexture(texture);

		} else {
			texture.clear();
		}
	}

	static constexpr const char* nodeName = "Height Map to Texture";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::heightmap;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtHeightMap heightMap;
	OtTexture texture;
};

static OtNodesFactoryRegister<OtHeightMapToTextureNode> registration;
