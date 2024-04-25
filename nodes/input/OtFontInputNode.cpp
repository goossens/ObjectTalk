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

#include "OtFont.h"
#include "OtFontAsset.h"
#include "OtPathTools.h"

#include "OtNodesFactory.h"


//
//	OtLoadFontNode
//

class OtLoadFontNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addOutputPin("Font", font)->addCustomRenderer([this](float width) {
			ImGui::SetNextItemWidth(width);
			auto old = serialize().dump();

			if (customInputRendering(width)) {
				if (asset.isNull()) {
					font.clear();
					needsEvaluating = true;

				} else if (asset.isReady()) {
					font = asset->getFont();
					needsEvaluating = true;

				} else {
					font.clear();
				}

				oldState = old;
				newState = serialize().dump();
				needsSaving = true;
			}
		}, fieldWidth);
	}

	// special rendering for input nodes
	inline bool customInputRendering(float width) override {
		ImGui::SetNextItemWidth(width);
		return asset.renderUI("##font");
	}

	// update state
	inline bool onUpdate() override {
		if (!asset.isNull() && asset->getFont() != font) {
			font = asset->getFont();
			return true;

		} else {
			return false;
		}
	}

	// (de)serialize node
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["path"] = OtPathRelative(asset.getPath(), basedir);
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		asset = OtPathGetAbsolute(*data, "path", basedir);

		if (asset.isNull()) {
			font.clear();

		} else if (asset.isReady()) {
			font = asset->getFont();
		}
	}

	static constexpr const char* nodeName = "Font Input";
	static constexpr int nodeCategory = OtNodeClass::input;
	static constexpr int nodeKind = OtNodeClass::fixed;
	static constexpr float fieldWidth = 180.0f;

protected:
	OtAsset<OtFontAsset> asset;
	OtFont font;
};

static OtNodesFactoryRegister<OtLoadFontNode> type;
