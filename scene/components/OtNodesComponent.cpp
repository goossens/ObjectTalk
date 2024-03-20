//	ObjectTalk Flowing Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <fstream>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtException.h"

#include "OtMessageBus.h"
#include "OtUi.h"

#include "OtNodesComponent.h"
#include "OtPathTools.h"


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
			auto basedir = OtPathGetParent(asset->getPath());
			nodes.loadFromString(asset->getText(), basedir);

			// build list of input nodes
			inputNodes.clear();

			nodes.eachNode([&](OtNode& node) {
				if (node->category == OtNodeClass::input) {
					inputNodes.emplace_back(node);
				}
			});

			// sort them by vertical position
			std::sort(inputNodes.begin(), inputNodes.end(), [](OtNode a, OtNode b) {
				return a->y < b->y;
			});

			// apply our settings
			applySettings(nlohmann::json::parse(savedSettings), &basedir);

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
		std::ofstream stream(path);
		stream << flowTemplate;
		stream.close();
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
	data["component"] = name;
	data["path"] = OtPathRelative(asset.getPath(), basedir);

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
	asset = OtPathGetAbsolute(data, "path", basedir);

	if (data.contains("settings")) {
		auto settings = data["settings"];
		applySettings(settings, basedir);
		savedSettings = settings.dump();

	} else {
		savedSettings = "{}";
	}
}


//
//	OtNodesComponent::applySettings
//

void OtNodesComponent::applySettings(nlohmann::json settings, std::string* basedir) {
	for (auto& node : inputNodes) {
		auto id = std::to_string(node->id);

		if (settings.contains(id)) {
			node->customDeserialize(&settings[id], basedir);
			node->needsEvaluating = true;
		}
	}
}
