//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

#include "OtLog.h"
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
	inline void addType(const char* typeName) {
		types.emplace_back(typeName);
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

class OtNodesFactory : OtSingleton<OtNodesFactory> {
public:
	// register a new type (name has to be globally unique in this factory, not just in category)
	static inline void registerType(const char* category, const char* name, std::function<OtNode()> constructor) {
		// find the category
		auto& factory = instance();

		auto i = std::find_if(factory.categories.begin(), factory.categories.end(), [category](OtNodeCategory& candidate) {
			return candidate.name == category;
		});

		// add type to (existing/new) category
		if (i == factory.categories.end()) {
			factory.categories.emplace_back(category);
			factory.categories.back().addType(name);
			std::sort(factory.categories.begin(), factory.categories.end());

		} else {
			i->addType(name);
		}

		// save constructor for fast access
		factory.constructors[name] = constructor;
	}

	// iterate through categories
	static inline void eachCategory(std::function<void(OtNodeCategory&)> callback) {
		for (auto& category : instance().categories) {
			callback(category);
		}
	}

	// create new node
	static inline OtNode createNode(const std::string& name) {
		// sanity check
		auto& factory = instance();

		if (factory.constructors.count(name) == 0) {
			OtLogError("Unknown node type[{}]", name);
		}

		// construct a new instance
		return factory.constructors[name]();
	}

private:
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
		OtNodesFactory::registerType(OtNodeClass::categoryNames[static_cast<int>(T::nodeCategory)], T::nodeName, []() {
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
