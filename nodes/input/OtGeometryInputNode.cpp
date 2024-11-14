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

#include "OtGeometry.h"
#include "OtGeometryAsset.h"
#include "OtPathTools.h"

#include "OtNodesFactory.h"


//
//	OtLoadGeometryNode
//

class OtLoadGeometryNode : public OtNodeClass {
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

	// update state
	inline bool onUpdate() override {
		if (!asset.isNull() && asset->getGeometry() != geometry) {
			geometry = asset->getGeometry();
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
			geometry.clear();

		} else if (asset.isReady()) {
			geometry = asset->getGeometry();
		}
	}

	static constexpr const char* nodeName = "Geometry Input";
	static constexpr int nodeCategory = OtNodeClass::input;
	static constexpr int nodeKind = OtNodeClass::fixed;

protected:
	OtAsset<OtGeometryAsset> asset;
	OtGeometry geometry;
};

static OtNodesFactoryRegister<OtLoadGeometryNode> type;
