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
//	OtGeometryInputNode
//

class OtGeometryInputNode : public OtNodeClass {
public:
	// constructor
	inline OtGeometryInputNode() : OtNodeClass(name, OtNodeClass::input) {}

	// configure node
	inline void configure() override {
		addOutputPin("Geometry", geometry)->addRenderer([this](float width) {
			ImGui::SetNextItemWidth(width);
			auto old = serialize().dump();

			if (asset.renderUI("##geometry")) {
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
		}, fieldWidth);
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

	static constexpr const char* name = "Geometry Input";
	static constexpr float fieldWidth = 180.0f;

protected:
	OtAsset<OtGeometryAsset> asset;
	OtGeometry geometry;
};

static OtNodesFactoryRegister<OtGeometryInputNode> type("Input");
