//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"


#include "OtImage.h"
#include "OtImageAsset.h"

#include "OtNodesFactory.h"


//
//	OtImageInputNode
//

class OtImageInputNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addOutputPin("Image", image)->addCustomRenderer([this](float width) {
			auto old = serialize().dump();

			if (customInputRendering(width)) {
				if (asset.isNull()) {
					image.clear();
					needsEvaluating = true;

				} else if (asset.isReady()) {
					image = asset->getImage();
					needsEvaluating = true;

				} else {
					image.clear();
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
		return asset.renderUI("##image");
	}

	// update status by checking for errors or load completion
	inline bool onUpdate() override {
		error.clear();

		if (asset.isNull() || asset.isMissing() || asset.isInvalid()) {
			image.clear();
			return false;

		} else if (asset->getImage() != image) {
			image = asset->getImage();
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
			image.clear();

		} else if (asset.isReady()) {
			image = asset->getImage();
		}
	}

	static constexpr const char* nodeName = "Image Input";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::input;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtAsset<OtImageAsset> asset;
	OtImage image;
};

static OtNodesFactoryRegister<OtImageInputNode> registration;
