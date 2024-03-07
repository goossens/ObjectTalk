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

#include "OtInstancesAsset.h"

#include "OtNodesFactory.h"


//
//	OtVirtualInstancesOutputNode
//

class OtVirtualInstancesOutputNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", instances)->addRenderer([&](float width) {
			auto old = serialize().dump();
			OtInstancesAsset* oldAsset = asset.isNull() ? nullptr : &(*asset);
			ImGui::SetNextItemWidth(width);

			if (asset.renderVirtualUI("##name")) {
				if (oldAsset && asset.isNull()) {
					oldAsset->clearInstances();
				}

				oldState = old;
				newState = serialize().dump();
				needsEvaluating = true;
				needsSaving = true;
			}
		}, fieldWidth);
	}

	// synchronize the asset with the incoming instances
	void onExecute() override {
		if (instances.isValid()) {
			if (!asset.isNull()) {
				asset->setInstances(instances);
			}

		} else if (!asset.isNull()) {
			asset->clearInstances();
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

	static constexpr const char* nodeName = "Save Instances To Virtual";
	static constexpr int nodeCategory = OtNodeClass::virtualizer;
	static constexpr int nodeKind = OtNodeClass::fixed;
	static constexpr float fieldWidth = 170.0f;

protected:
	OtInstances instances;
	OtAsset<OtInstancesAsset> asset;
};

static OtNodesFactoryRegister<OtVirtualInstancesOutputNode> type;
