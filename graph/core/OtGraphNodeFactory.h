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
#include <unordered_map>
#include <vector>

#include "OtGraphNode.h"
#include "OtGraphNodeCategory.h"


//
//	OtGraphNodeFactory
//

class OtGraphNodeFactory {
public:
	// register a new type (name has to be globally unique in this factory, not just in category)
	inline void registerType(const char* category, const char* name, std::function<OtGraphNode()> constructor) {
		// find the category
		auto i = std::find_if(categories.begin(), categories.end(), [category](OtGraphNodeCategory& candidate) {
			return candidate.name == category;
		});

		// add type to (existing/new) category
		if (i == categories.end()) {
			categories.emplace_back(category);
			categories.back().addType(name, constructor);
			std::sort(categories.begin(), categories.end());

		} else {
			i->addType(name, constructor);
		}

		// save constructor for fast access
		constructors[name] = constructor;
	}

	// iterate through categories
	inline void eachCategory(std::function<void(OtGraphNodeCategory&)> callback) {
		for (auto& category : categories) {
			callback(category);
		}
	}

	// create new node
	inline OtGraphNode createNode(const std::string& name) {
		return constructors[name]();
	}

	// properties
	std::vector<OtGraphNodeCategory> categories;
	std::unordered_map<std::string, std::function<OtGraphNode()>> constructors;
};
