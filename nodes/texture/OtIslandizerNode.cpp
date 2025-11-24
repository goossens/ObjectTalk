//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
	inline void customRendering(float itemWidth) override {
		auto old = serialize().dump();
		ImGui::SetNextItemWidth(itemWidth);

		if (OtUi::selectorEnum("##distance", &distance, OtIslandizer::distanceFunctions, OtIslandizer::distanceFunctionCount)) {
			oldState = old;
			newState = serialize().dump();
			needsEvaluating = true;
			needsSaving = true;
		}
	}

	inline float getCustomRenderingWidth() override {
		return 200.0f;
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["distance"] = distance;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		distance = data->value("distance", OtIslandizer::DistanceFunction::squareBump);
	}

	// run filter
	inline void onFilter(OtTexture& input, OtTexture& output) override {
		islandizer.setDistanceFunction(distance);
		islandizer.render(input, output);
	}

	static constexpr const char* nodeName = "Islandizer";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::texture;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtIslandizer islandizer;
	OtIslandizer::DistanceFunction distance = OtIslandizer::DistanceFunction::squareBump;
};

static OtNodesFactoryRegister<OtIslandizerNode> registration;
