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

#include "OtTextureAsset.h"

#include "OtNodesFactory.h"


//
//	OtVirtualTextureOutputNode
//

class OtVirtualTextureOutputNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", texture)->addRenderer([&](float width) {
			auto old = serialize().dump();
			OtTextureAsset* oldAsset = asset.isNull() ? nullptr : &(*asset);
			ImGui::SetNextItemWidth(width);

			if (asset.renderVirtualUI("##name")) {
				if (oldAsset && asset.isNull()) {
					oldAsset->clearTexture();
				}

				oldState = old;
				newState = serialize().dump();
				needsEvaluating = true;
				needsSaving = true;
			}
		}, fieldWidth);
	}

	// synchronize the asset with the incoming texture
	void onExecute() override {
		if (texture.isValid()) {
			if (!asset.isNull()) {
				asset->setTexture(texture);
			}

		} else if (!asset.isNull()) {
			asset->clearTexture();
		}
	}

	// (de)serialize node
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		auto t = asset.getPath();
		(*data)["path"] = asset.getPath();
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		asset = data->value("path", "");
	}

	static constexpr const char* nodeName = "Save Texture To Virtual";
	static constexpr int nodeCategory = OtNodeClass::virtualizer;
	static constexpr int nodeKind = OtNodeClass::fixed;
	static constexpr float fieldWidth = 170.0f;

protected:
	OtTexture texture;
	OtAsset<OtTextureAsset> asset;
};

static OtNodesFactoryRegister<OtVirtualTextureOutputNode> type;