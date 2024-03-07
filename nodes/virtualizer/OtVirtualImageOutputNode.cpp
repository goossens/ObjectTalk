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

#include "OtImageAsset.h"

#include "OtNodesFactory.h"


//
//	OtVirtualImageOutputNode
//

class OtVirtualImageOutputNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", image)->addRenderer([&](float width) {
			auto old = serialize().dump();
			OtImageAsset* oldAsset = asset.isNull() ? nullptr : &(*asset);
			ImGui::SetNextItemWidth(width);

			if (asset.renderVirtualUI("##name")) {
				if (oldAsset && asset.isNull()) {
					oldAsset->clearImage();
				}

				oldState = old;
				newState = serialize().dump();
				needsEvaluating = true;
				needsSaving = true;
			}
		}, fieldWidth);
	}

	// synchronize the asset with the incoming image
	void onExecute() override {
		if (image.isValid()) {
			if (!asset.isNull()) {
				asset->setImage(image);
			}

		} else if (!asset.isNull()) {
			asset->clearImage();
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

	static constexpr const char* nodeName = "Save Image To Virtual";
	static constexpr int nodeCategory = OtNodeClass::virtualizer;
	static constexpr int nodeKind = OtNodeClass::fixed;
	static constexpr float fieldWidth = 170.0f;

protected:
	OtImage image;
	OtAsset<OtImageAsset> asset;
};

static OtNodesFactoryRegister<OtVirtualImageOutputNode> type;
