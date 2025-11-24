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
	inline OtTexture::Format getOutputFormat() override { return OtTexture::Format::rgba8; }

	// run filter
	inline void onFilter(OtTexture& input, OtTexture& output) override {
		mapper.render(input, output);
	}

	static constexpr const char* nodeName = "HeightMap to NormalMap";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::texture;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtNormalMapper mapper;
};

static OtNodesFactoryRegister<OtHeightmapToNormalMapNode> registration;
