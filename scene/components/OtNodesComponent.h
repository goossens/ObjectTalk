//	ObjectTalk Flowing Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <vector>

#include "nlohmann/json_fwd.hpp"

#include "OtNodes.h"
#include "OtNodesAsset.h"


//
//	OtNodesComponent
//

class OtNodesComponent {
public:
	// constructor/destructor
	OtNodesComponent();
	~OtNodesComponent();

	// UI to change component properties
	bool renderUI();

	// (de)serialize component
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

	// component name
	static constexpr char const* name = "Nodes";

	// stored properties
	OtAsset<OtNodesAsset> asset;

	// runtime properties
	OtNodes nodes;
	std::vector<OtNode> inputNodes;
	std::string savedSettings = "{}";
	std::string savedBasedir;

	void applySettings();
};
