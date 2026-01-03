//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "nlohmann/json_fwd.hpp"

#include "OtIdentifier.h"

#include "OtScriptAsset.h"


//
//	OtScriptComponent
//

class OtScriptComponent {
public:
	// UI to change component properties
	bool renderUI();

	// load and compile the script
	void process();

	// runtime methods
	void create();
	void update();

	// (de)serialize component
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

	// component name
	static constexpr char const* name = "Script";

	// stored properties
	OtAsset<OtScriptAsset> script;

	// runtime properties
	OtObject instance;
	bool hasCreateMethod;
	bool hasUpdateMethod;

	OtID createSymbol = OtIdentifier::create("create");
	OtID updateSymbol = OtIdentifier::create("update");
};
