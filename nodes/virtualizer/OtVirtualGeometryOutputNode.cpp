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

#include "OtGeometryAsset.h"

#include "OtNodesFactory.h"


//
//	OtVirtualGeometryOutputNode
//

class OtVirtualGeometryOutputNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", geometry)->addRenderer([&](float width) {
			auto old = serialize().dump();
			OtGeometryAsset* oldAsset = asset.isNull() ? nullptr : &(*asset);
			ImGui::SetNextItemWidth(width);

			if (asset.renderVirtualUI("##name")) {
				if (oldAsset && asset.isNull()) {
					oldAsset->clearGeometry();
				}

				oldState = old;
				newState = serialize().dump();
				needsEvaluating = true;
				needsSaving = true;
			}
		}, fieldWidth);
	}

	// synchronize the asset with the incoming geometry
	void onExecute() override {
		if (geometry.isValid()) {
			if (!asset.isNull()) {
				asset->setGeometry(geometry);
			}

		} else if (!asset.isNull()) {
			asset->clearGeometry();
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

	static constexpr const char* nodeName = "Save Geometry To Virtual";
	static constexpr int nodeCategory = OtNodeClass::output;
	static constexpr int nodeKind = OtNodeClass::virtualizer;
	static constexpr float fieldWidth = 170.0f;

protected:
	OtGeometry geometry;
	OtAsset<OtGeometryAsset> asset;
};

static OtNodesFactoryRegister<OtVirtualGeometryOutputNode> type;
