//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "nlohmann/json.hpp"

#include "OtImage.h"
#include "OtMap.h"
#include "OtUi.h"

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
		addOutputPin("Image", image);
	}

	// render custom fields
	inline void customRendering(float itemWidth) override {
		ImGui::SetNextItemWidth(itemWidth);
		auto old = serialize().dump();

		if (OtUi::selectorEnum("##renderType", &renderType, OtMap::renderTypes, OtMap::renderTypeCount)) {
			oldState = old;
			newState = serialize().dump();
			needsEvaluating = true;
			needsSaving = true;
		}
	}

	inline float getCustomRenderingWidth() override {
		return 180.0f;
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["renderType"] = renderType;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		renderType = data->value("renderType", OtMap::RenderType::biomes);
	}

	// validate input parameters
	inline void onValidate() override {
		size = std::clamp(size, 32, 1024);
	}

	// generate the image
	inline void onExecute() override {
		if (map.isValid()) {
			map.render(image, size, renderType);

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
	OtMap::RenderType renderType = OtMap::RenderType::biomes;

	// map component
	OtMap map;
	OtImage image;
};


static OtNodesFactoryRegister<OtMapToImageNode> registration;
