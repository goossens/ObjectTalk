//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtFrameBuffer.h"
#include "OtIslandizer.h"
#include "OtNormalMapper.h"
#include "OtSeamlessTile.h"
#include "OtTexture.h"
#include "OtUi.h"

#include "OtGraphNode.h"
#include "OtTextureFilterNodes.h"


//
//	OtTextureFilterNode
//

class OtTextureFilterNode : public OtGraphNodeClass {
public:
	// constructor
	inline OtTextureFilterNode(const char* name) : OtGraphNodeClass(name, OtGraphNodeClass::filter) {
		framebuffer.initialize(getColorTextureType(), getDepthTextureType());
	}

	// configure output framebuffer
	inline virtual int getColorTextureType() { return OtFrameBuffer::rgbaFloat32Texture; }
	inline virtual int getDepthTextureType() { return OtFrameBuffer::noTexture; }

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
	OtFrameBuffer framebuffer;
	OtTexture inputTexture;
	OtTexture outputTexture;
	int version = 1;
};


//
//	OtHeightmapToNormalMap
//

class OtHeightmapToNormalMap : public OtTextureFilterNode {
public:
	// constructor
	inline OtHeightmapToNormalMap() : OtTextureFilterNode(name) {}

	// run filter
	void onFilter() override {
		mapper.render(inputTexture, framebuffer);
	}

	static constexpr const char* name = "HeightMap to NormalMap";

	// properties
	OtNormalMapper mapper;
};


//
//	OtSeamlessTileFilter
//

class OtSeamlessTileFilter : public OtTextureFilterNode {
public:
	// constructor
	inline OtSeamlessTileFilter() : OtTextureFilterNode(name) {}

	// run filter
	void onFilter() override {
		tiler.render(inputTexture, framebuffer);
	}

	static constexpr const char* name = "Seamless Tiler";

	// properties
	OtSeamlessTile tiler;
};


//
//	OtIslandizerFilter
//

class OtIslandizerFilter : public OtTextureFilterNode {
public:
	// constructor
	inline OtIslandizerFilter() : OtTextureFilterNode(name) {}

	// configure output framebuffer
	inline int getColorTextureType() override { return OtFrameBuffer::rFloat32Texture; }

	// rendering custom fields
	void customRendering(float width) override {
		auto old = serialize().dump();
		ImGui::SetNextItemWidth(width);

		if (OtUiSelectorEnum("##distance", distance, OtIslandizer::distanceFunctions, OtIslandizer::distanceFunctionCount)) {
			oldState = old;
			newState = serialize().dump();
			needsEvaluating = true;
			needsSaving = true;
		}
	}

	float getCustomRenderingWidth() override {
		return fieldWidth;
	}

	float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize input
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["distance"] = distance;
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		distance = data->value("distance", OtIslandizer::squareBump);
	}

	// run filter
	void onFilter() override {
		islandizer.setDistanceFunction(distance);
		islandizer.render(inputTexture, framebuffer);
	}

	static constexpr const char* name = "Islandizer";
	static constexpr float fieldWidth = 200.0f;

	// properties
	OtIslandizer islandizer;
	int distance = OtIslandizer::squareBump;
};


//
//	OtTextureFilterNodesRegister
//

#define REGISTER(CLASS) \
	graph.registerNodeType<CLASS>("Texture Filters", CLASS::name)

void OtTextureFilterNodesRegister(OtGraph &graph) {
	REGISTER(OtHeightmapToNormalMap);
	REGISTER(OtIslandizerFilter);
	REGISTER(OtSeamlessTileFilter);
}
