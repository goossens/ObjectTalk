//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

#include "OtGraphNode.h"
#include "OtGraphNodeType.h"


//
//	OtGraphNodeCategory
//

class OtGraphNodeCategory {
public:
	// constructor
	inline OtGraphNodeCategory(const char* n) : name(n) {}

	// add a new type
	inline void addType(const char* name, std::function<OtGraphNode()> constructor) {
		types.emplace_back(name, constructor);
		std::sort(types.begin(), types.end());
	}

	// compare categories
	inline bool operator < (const OtGraphNodeCategory& category) const {
		return name < category.name;
	}

	// iterate through types
	inline void eachType(std::function<void(OtGraphNodeType&)> callback) {
		for (auto& type : types) {
			callback(type);
		}
	}

	// properties
	std::string name;
	std::vector<OtGraphNodeType> types;
};
