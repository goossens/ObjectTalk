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
#include <vector>

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
//	OtNodesComponent::renderUI
//

bool OtNodesComponent::renderUI() {
	bool changed = asset.renderUI("Path##NodesPath", [](const std::string& path) {
		// create a new nodes file
		std::ofstream stream(path);
		stream << flowTemplate;
		stream.close();
	});

/*
	if (asset.isReady()) {
		auto& nodes = asset->getNodes();
		std::vector<OtNode> inputNodes;

		nodes.eachNode([&](OtNode& node) {
			if (node->category == OtNodeClass::input) {
				inputNodes.emplace_back(node);
			}
		});

		std::sort(inputNodes.begin(), inputNodes.end(), [](OtNode a, OtNode b) {
			return a->y < b->y;
		});

		for (auto& node : inputNodes) {
			node->eachPin([node](OtNodesPin& pin) {
				ImGui::PushID(pin.get());
				pin->render(ImGui::CalcItemWidth());
				ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
				ImGui::TextUnformatted(node->title.c_str());
				ImGui::PopID();
			});
		}
	}
*/

	return changed;
}


//
//	OtNodesComponent::serialize
//

nlohmann::json OtNodesComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["path"] = OtPathRelative(asset.getPath(), basedir);
	return data;
}


//
//	OtNodesComponent::deserialize
//

void OtNodesComponent::deserialize(nlohmann::json data, std::string* basedir) {
	asset = OtPathGetAbsolute(data, "path", basedir);
}
