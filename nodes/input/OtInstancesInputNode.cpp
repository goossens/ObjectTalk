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

#include "OtInstances.h"
#include "OtInstancesAsset.h"
#include "OtPathTools.h"

#include "OtNodesFactory.h"


//
//	OtLoadInstancesNode
//

class OtLoadInstancesNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addOutputPin("Instances", instances)->addRenderer([this](float width) {
			ImGui::SetNextItemWidth(width);
			auto old = serialize().dump();

			if (customInputRendering(width)) {
				if (asset.isNull()) {
					instances.clear();
					needsEvaluating = true;

				} else if (asset.isReady()) {
					instances = asset->getInstances();
					needsEvaluating = true;

				} else {
					instances.clear();
				}

				oldState = old;
				newState = serialize().dump();
				needsSaving = true;
			}
		}, fieldWidth);
	}

	// special rendering for input nodes
	inline bool customInputRendering(float width) override {
		ImGui::SetNextItemWidth(width);
		return asset.renderUI("##instances");
	}

	// update state
	inline bool onUpdate() override {
		if (!asset.isNull() && asset->getInstances() != instances) {
			instances = asset->getInstances();
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
			instances.clear();

		} else if (asset.isReady()) {
			instances = asset->getInstances();
		}
	}

	static constexpr const char* nodeName = "Instances Input";
	static constexpr int nodeCategory = OtNodeClass::input;
	static constexpr int nodeKind = OtNodeClass::fixed;
	static constexpr float fieldWidth = 180.0f;

protected:
	OtAsset<OtInstancesAsset> asset;
	OtInstances instances;
};

static OtNodesFactoryRegister<OtLoadInstancesNode> type;
