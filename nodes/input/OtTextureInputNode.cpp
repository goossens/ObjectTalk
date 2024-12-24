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


#include "OtTexture.h"
#include "OtTextureAsset.h"

#include "OtNodesFactory.h"


//
//	OtLoadTextureNode
//

class OtLoadTextureNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addOutputPin("Texture", texture)->addCustomRenderer([this](float width) {
			ImGui::SetNextItemWidth(width);
			auto old = serialize().dump();

			if (customInputRendering(width)) {
				if (asset.isNull()) {
					texture.clear();
					needsEvaluating = true;

				} else if (asset.isReady()) {
					texture = asset->getTexture();
					needsEvaluating = true;

				} else {
					texture.clear();
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
		return asset.renderUI("##texture");
	}

	// update state
	inline bool onUpdate() override {
		if (!asset.isNull() && asset->getTexture() != texture) {
			texture = asset->getTexture();
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
			texture.clear();

		} else if (asset.isReady()) {
			texture = asset->getTexture();
			needsEvaluating = true;
		}
	}

	static constexpr const char* nodeName = "Texture Input";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::input;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	OtAsset<OtTextureAsset> asset;
	OtTexture texture;
};

static OtNodesFactoryRegister<OtLoadTextureNode> type;
