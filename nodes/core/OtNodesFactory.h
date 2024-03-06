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

#include "OtException.h"
#include "OtSingleton.h"

#include "OtNode.h"


//
//	OtNodeCategory
//

class OtNodeCategory {
public:
	// constructor
	inline OtNodeCategory(const char* n) : name(n) {}

	// add a new type
	inline void addType(const char* name) {
		types.emplace_back(name);
		std::sort(types.begin(), types.end());
	}

	// compare categories
	inline bool operator<(const OtNodeCategory& category) const {
		return name < category.name;
	}

	// iterate through types
	inline void eachType(std::function<void(const std::string&)> callback) {
		for (auto& type : types) {
			callback(type);
		}
	}

	// properties
	std::string name;
	std::vector<std::string> types;
};


//
//	OtNodesFactory
//

class OtNodesFactory : public OtSingleton<OtNodesFactory> {
public:
	// register a new type (name has to be globally unique in this factory, not just in category)
	inline void registerType(const char* category, const char* name, std::function<OtNode()> constructor) {
		// find the category
		auto i = std::find_if(categories.begin(), categories.end(), [category](OtNodeCategory& candidate) {
			return candidate.name == category;
		});

		// add type to (existing/new) category
		if (i == categories.end()) {
			categories.emplace_back(category);
			categories.back().addType(name);
			std::sort(categories.begin(), categories.end());

		} else {
			i->addType(name);
		}

		// save constructor for fast access
		constructors[name] = constructor;
	}

	// iterate through categories
	inline void eachCategory(std::function<void(OtNodeCategory&)> callback) {
		for (auto& category : categories) {
			callback(category);
		}
	}

	// create new node
	inline OtNode createNode(const std::string& name) {
		// sanity check
		if (constructors.count(name) == 0) {
			OtError("Unknown node type[{}]", name);
		}

		// construct a new instance
		return constructors[name]();
	}

	// properties
	std::vector<OtNodeCategory> categories;
	std::unordered_map<std::string, std::function<OtNode()>> constructors;
};


//
//	OtNodesFactoryRegister
//

template <typename T>
class OtNodesFactoryRegister {
public:
	// sanity check
	static_assert(std::is_base_of<OtNodeClass, T>::value, "Class is not derived from OtNodeClass");

	// constructor
	OtNodesFactoryRegister() {
		OtNodesFactory::instance()->registerType(OtNodeClass::categoryNames[T::nodeCategory], T::nodeName, []() {
			auto node = std::make_shared<T>();
			node->type = T::nodeName;
			node->title = T::nodeName;
			node->category = T::nodeCategory;
			node->kind = T::nodeKind;
			node->configure();
			return node;
		});
	}
};