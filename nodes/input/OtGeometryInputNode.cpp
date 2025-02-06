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


#include "OtGeometry.h"
#include "OtGeometryAsset.h"

#include "OtNodesFactory.h"


//
//	OtGeometryInputNode
//

class OtGeometryInputNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addOutputPin("Geometry", geometry)->addCustomRenderer([this](float width) {
			auto old = serialize().dump();

			if (customInputRendering(width)) {
				if (asset.isNull()) {
					geometry.clear();
					needsEvaluating = true;

				} else if (asset.isReady()) {
					geometry = asset->getGeometry();
					needsEvaluating = true;

				} else {
					geometry.clear();
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
		return asset.renderUI("##geometry");
	}

	// update status by checking for errors or load completion
	inline bool onUpdate() override {
		error.clear();

		if (asset.isNull() || asset.isMissing() || asset.isInvalid()) {
			geometry.clear();
			return false;

		} else if (asset->getGeometry() != geometry) {
			geometry = asset->getGeometry();
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
			geometry.clear();

		} else if (asset.isReady()) {
			geometry = asset->getGeometry();
		}
	}

	static constexpr const char* nodeName = "Geometry Input";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::input;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	OtAsset<OtGeometryAsset> asset;
	OtGeometry geometry;
};

static OtNodesFactoryRegister<OtGeometryInputNode> type;
