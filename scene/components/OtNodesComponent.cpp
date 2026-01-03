//	ObjectTalk Flowing Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtMessageBus.h"
#include "OtText.h"

#include "OtUi.h"

#include "OtNodesComponent.h"
#include "OtPath.h"


//
//	New flow template
//

static const char* flowTemplate = "\
{\n\
	\"metadata\": {},\n\
	\"nodes\": [],\n\
	\"links\": []\n\
}\n";


//
//	OtNodesComponent::OtNodesComponent
//

OtNodesComponent::OtNodesComponent() {
	asset.onChange([&]() {
		if (asset.isReady()) {
			// load nodes
			auto basedir = OtPath::getParent(asset->getPath());
			nodes.loadFromString(asset->getText(), basedir);

			// build list of input nodes
			inputNodes.clear();

			nodes.eachNode([&](OtNode node) {
				if (node->category == OtNodeClass::Category::input) {
					inputNodes.emplace_back(node);
				}
			});

			// sort them by vertical position
			std::sort(inputNodes.begin(), inputNodes.end(), [](OtNode a, OtNode b) {
				return a->y < b->y;
			});

			// apply our settings
			applySettings();

		} else {
			// no nodes specified, just clear things
			nodes.clear();
			inputNodes.clear();
		}
	});
}


//
//	OtNodesComponent::~OtNodesComponent
//

OtNodesComponent::~OtNodesComponent() {
	asset.onChange(nullptr);
}


//
//	OtNodesComponent::renderUI
//

bool OtNodesComponent::renderUI() {
	bool changed = asset.renderUI("Path##NodesPath", [](const std::string& path) {
		// create a new nodes file
		OtText::save(path, flowTemplate);
	});

	// show all input nodes (if required)
	if (inputNodes.size() && ImGui::TreeNode("Parameters")) {
		auto width = ImGui::CalcItemWidth();
		auto spacing = ImGui::GetStyle().ItemInnerSpacing.x;

		for (auto& node : inputNodes) {
			ImGui::PushID(node.get());
			changed |= node->customInputRendering(width);
			ImGui::SameLine(0.0f, spacing);
			ImGui::TextUnformatted(node->title.c_str());
			ImGui::PopID();
		}

		ImGui::TreePop();
	}

	return changed;
}


//
//	OtNodesComponent::serialize
//

nlohmann::json OtNodesComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["path"] = OtAssetSerialize(asset.getPath(), basedir);

	auto settings = nlohmann::json::object();

	for (auto& node : inputNodes) {
		auto values = nlohmann::json::object();
		node->customSerialize(&values, basedir);
		settings[std::to_string(node->id)] = values;
	}

	data["settings"] = settings;
	return data;
}


//
//	OtNodesComponent::deserialize
//

void OtNodesComponent::deserialize(nlohmann::json data, std::string* basedir) {
	asset = OtAssetDeserialize(&data, "path", basedir);
	savedBasedir = basedir ? *basedir : "";

	if (data.contains("settings")) {
		savedSettings =  data["settings"].dump();
		applySettings();

	} else {
		savedSettings = "{}";
	}
}


//
//	OtNodesComponent::applySettings
//

void OtNodesComponent::applySettings() {
	auto settings = nlohmann::json::parse(savedSettings);

	for (auto& node : inputNodes) {
		auto id = std::to_string(node->id);

		if (settings.contains(id)) {
			node->customDeserialize(&settings[id], savedBasedir.size() ? &savedBasedir : nullptr);
			node->needsEvaluating = true;
		}
	}
}
