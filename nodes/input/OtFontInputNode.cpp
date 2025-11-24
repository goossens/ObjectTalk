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


#include "OtFont.h"
#include "OtFontAsset.h"

#include "OtNodesFactory.h"


//
//	OtFontInputNode
//

class OtFontInputNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addOutputPin("Font", font)->addCustomRenderer([this](float width) {
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
		}, 180.0f);
	}

	// special rendering for input nodes
	inline bool customInputRendering(float width) override {
		ImGui::SetNextItemWidth(width);
		return asset.renderUI("##font");
	}

	// update status by checking for errors or load completion
	inline bool onUpdate() override {
		error.clear();

		if (asset.isNull() || asset.isMissing() || asset.isInvalid()) {
			font.clear();
			return false;

		} else if (asset->getFont() != font) {
			font = asset->getFont();
			return true;

		} else {
			return false;
		}
	}

	// (de)serialize node
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["path"] = OtAssetSerialize(asset.getPath(), basedir);
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		asset = OtAssetDeserialize(data, "path", basedir);

		if (asset.isNull()) {
			font.clear();

		} else if (asset.isReady()) {
			font = asset->getFont();
		}
	}

	static constexpr const char* nodeName = "Font Input";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::input;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtAsset<OtFontAsset> asset;
	OtFont font;
};

static OtNodesFactoryRegister<OtFontInputNode> registration;
