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


#include "OtHeightMap.h"
#include "OtHeightMapAsset.h"

#include "OtNodesFactory.h"


//
//	OtHeightMapInputNode
//

class OtHeightMapInputNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addOutputPin("HeightMap", heightMap)->addCustomRenderer([this](float width) {
			auto old = serialize().dump();

			if (customInputRendering(width)) {
				if (asset.isNull()) {
					heightMap.clear();
					needsEvaluating = true;

				} else if (asset.isReady()) {
					heightMap = asset->getHeightMap();
					needsEvaluating = true;

				} else {
					heightMap.clear();
				}

				oldState = old;
				newState = serialize().dump();
				needsSaving = true;
			}
		}, OtUi::size(9.0f));
	}

	// special rendering for input nodes
	inline bool customInputRendering(float width) override {
		ImGui::SetNextItemWidth(width);
		return asset.renderUI("##heightMap");
	}

	// update status by checking for errors or load completion
	inline bool onUpdate() override {
		error.clear();

		if (asset.isNull() || asset.isMissing() || asset.isInvalid()) {
			heightMap.clear();
			return false;

		} else if (asset->getHeightMap() != heightMap) {
			heightMap = asset->getHeightMap();
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
			heightMap.clear();

		} else if (asset.isReady()) {
			heightMap = asset->getHeightMap();
		}
	}

	static constexpr const char* nodeName = "Height Map Input";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::input;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtAsset<OtHeightMapAsset> asset;
	OtHeightMap heightMap;
};

static OtNodesFactoryRegister<OtHeightMapInputNode> registration;
