//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtNormalMapper.h"

#include "OtNodesFactory.h"
#include "OtTextureFilterNode.h"


//
//	OtHeightmapToNormalMapNode
//

class OtHeightmapToNormalMapNode : public OtTextureFilterNode {
public:
	// get output format
	inline int getOutputFormat() override { return OtTexture::rgba8Texture; }

	// run filter
	inline void onFilter(OtTexture& input, OtFrameBuffer& output) override {
		mapper.render(input, output);
	}

	static constexpr const char* nodeName = "HeightMap to NormalMap";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::texture;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

	// properties
	OtNormalMapper mapper;
};

static OtNodesFactoryRegister<OtHeightmapToNormalMapNode> registration;
