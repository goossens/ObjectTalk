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

#include "OtPathTools.h"
#include "OtTexture.h"
#include "OtTextureAsset.h"

#include "OtNodesFactory.h"


//
//	OtTextureInputNode
//

class OtTextureInputNode : public OtNodeClass {
public:
	// constructor
	inline OtTextureInputNode() : OtNodeClass(name, OtNodeClass::input) {}

	// configure node
	inline void configure() override {
		addOutputPin("Texture", texture)->addRenderer([this](float width) {
			ImGui::SetNextItemWidth(width);
			auto old = serialize().dump();

			if (asset.renderUI("##image")) {
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
		}, fieldWidth);
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

	// (de)serialize input
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["image"] = OtPathRelative(asset.getPath(), basedir);
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		asset = OtPathGetAbsolute(*data, "image", basedir);

		if (asset.isNull()) {
			texture.clear();

		} else if (asset.isReady()) {
			texture = asset->getTexture();
			needsEvaluating = true;
		}
	}

	static constexpr const char* name = "Texture Input";
	static constexpr float fieldWidth = 180.0f;

protected:
	OtAsset<OtTextureAsset> asset;
	OtTexture texture;
};

static OtNodesFactoryRegister<OtTextureInputNode> type("Input");
