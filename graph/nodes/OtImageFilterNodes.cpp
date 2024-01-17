//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFrameBuffer.h"
#include "OtNormalMapper.h"
#include "OtSeamlessTile.h"
#include "OtTexture.h"

#include "OtGraphNode.h"
#include "OtImageFilterNodes.h"


//
//	OtImageFilterNode
//

class OtImageFilterNode : public OtGraphNodeClass {
public:
	// constructor
	inline OtImageFilterNode(const char* name) : OtGraphNodeClass(name, OtGraphNodeClass::filter) {}

	// configure node
	inline void configure() override {
		addInputPin("Input", inputTexture);
		addOutputPin("Output", outputTexture);
	}

	// running the HeightMap to NormalMap filter
	void onExecute() override {
		// do we have a valid input
		if (inputTexture.isValid()) {
			// ensure framebuffer has right size
			framebuffer.update(inputTexture.getWidth(), inputTexture.getHeight());

			// run filter
			onFilter();

			// manage output (versions number are used to detect changes down the line)
			outputTexture = framebuffer.getColorTexture();
			outputTexture.setVersion(version++);

		} else {
			outputTexture.clear();
		}
	}

	// the actual filter execution (to be overriden by subclasses)
	virtual void onFilter() = 0;

	// properties
	OtFrameBuffer framebuffer{OtFrameBuffer::rgbaFloat32Texture};
	OtTexture inputTexture;
	OtTexture outputTexture;
	int version = 1;
};


//
//	OtHeightmapToNormalMap
//

class OtHeightmapToNormalMap : public OtImageFilterNode {
public:
	// constructor
	inline OtHeightmapToNormalMap() : OtImageFilterNode(name) {}

	// run filter
	void onFilter() {
		mapper.render(inputTexture, framebuffer);
	}

	static constexpr const char* name = "HeightMap to NormalMap";

	// properties
	OtNormalMapper mapper;
};


//
//	OtSeamlessTileFilter
//

class OtSeamlessTileFilter : public OtImageFilterNode {
public:
	// constructor
	inline OtSeamlessTileFilter() : OtImageFilterNode(name) {}

	// run filter
	void onFilter() {
		tiler.render(inputTexture, framebuffer);
	}

	static constexpr const char* name = "Seamless Tiler";

	// properties
	OtSeamlessTile tiler;
};


//
//	OtImageFilterNodesRegister
//

#define REGISTER(CLASS) \
	graph.registerNodeType<CLASS>("Image Filters", CLASS::name)

void OtImageFilterNodesRegister(OtGraph &graph) {
	REGISTER(OtHeightmapToNormalMap);
	REGISTER(OtSeamlessTileFilter);
}
