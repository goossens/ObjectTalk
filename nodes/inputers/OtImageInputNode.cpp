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

#include "OtImage.h"
#include "OtImageAsset.h"
#include "OtPathTools.h"

#include "OtNodesFactory.h"


//
//	OtImageInputNode
//

class OtImageInputNode : public OtNodeClass {
public:
	// constructor
	inline OtImageInputNode() : OtNodeClass(name, OtNodeClass::input) {}

	// configure node
	inline void configure() override {
		addOutputPin("Image", image)->addRenderer([this](float width) {
			ImGui::SetNextItemWidth(width);
			auto old = serialize().dump();

			if (asset.renderUI("##image")) {
				if (asset.isNull()) {
					image.clear();
					needsEvaluating = true;

				} else if (asset.isReady()) {
					image = asset->getImage();
					needsEvaluating = true;

				} else {
					image.clear();
					loading = true;
				}

				oldState = old;
				newState = serialize().dump();
				needsSaving = true;
			}
		}, fieldWidth);
	}

	// update state
	inline void onUpdate() override {
		if (loading && asset.isReady()) {
			image = asset->getImage();
			needsEvaluating = true;
			loading = false;

		} else if (asset.isReady() && asset->getImage().getVersion() != version) {
			image = asset->getImage();
			version = image.getVersion();
			needsEvaluating = true;
		}
	}

	// (de)serialize input
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["image"] = OtPathRelative(asset.getPath(), basedir);
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		asset = OtPathGetAbsolute(*data, "image", basedir);

		if (asset.isNull()) {
			image.clear();

		} else if (asset.isReady()) {
			image = asset->getImage();
			needsEvaluating = true;

		} else {
			loading = true;
		}
	}

	static constexpr const char* name = "Image Input";
	static constexpr float fieldWidth = 180.0f;

protected:
	OtAsset<OtImageAsset> asset;
	OtImage image;
	int version = 0;
	bool loading = false;
};

static OtNodesFactoryRegister<OtImageInputNode> type("Input");
