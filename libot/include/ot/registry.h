//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <unordered_map>

#include "exception.h"


//
//	OtRegistry
//

template<class T>
class OtRegistry {
public:
	// add a new member to the registry
	void set(const std::string& name, T member) {
		if (registry.count(name)) {
			OtExcept("Member [%s] already in registry", name.c_str());
		}

		registry[name] = member;
	}

	// get member from registry
	T get(const std::string& name) {
		if (!registry.count(name)) {
			OtExcept("Member [%s] not in registry", name.c_str());
		}

		return registry[name];
	}

	// see if registry has specified member
	bool has(const std::string& name) {
		return registry.count(name) != 0;
	}

private:
	// registry member
	std::unordered_map<std::string, T> registry;
};
