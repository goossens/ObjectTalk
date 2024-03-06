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

#include "OtIslandizer.h"
#include "OtUi.h"

#include "OtNodesFactory.h"
#include "OtTextureFilterNode.h"


//
//	OtIslandizerNode
//

class OtIslandizerNode : public OtTextureFilterNode {
public:
	// render custom fields
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

	// (de)serialize node
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

	static constexpr const char* nodeName = "Islandizer";
	static constexpr int nodeCategory = OtNodeClass::filter;
	static constexpr int nodeKind = OtNodeClass::fixed;
	static constexpr float fieldWidth = 200.0f;

	// properties
	OtIslandizer islandizer;
	int distance = OtIslandizer::squareBump;
};

static OtNodesFactoryRegister<OtIslandizerNode> type;
