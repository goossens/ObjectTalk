//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
	// constructor
	inline OtHeightmapToNormalMapNode() : OtTextureFilterNode(name) {}

	// get output format
	inline int getOutputFormat() override { return OtTexture::rgba8Texture; }

	// run filter
	void onFilter() override {
		mapper.render(inputTexture, framebuffer);
	}

	static constexpr const char* name = "HeightMap to NormalMap";

	// properties
	OtNormalMapper mapper;
};

static OtNodesFactoryRegister<OtHeightmapToNormalMapNode> type("Filters");
