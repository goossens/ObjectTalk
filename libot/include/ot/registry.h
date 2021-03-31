//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <unordered_map>
#include <vector>


//
//	OtRegistry
//

template<class T>
class OtRegistry {
public:
	// add a new member to the registry
	void set(const std::string& name, T member) {
		if (index.count(name)) {
			OtExcept("Member [%s] already in registry", name.c_str());
		}

		index[name] = members.size();
		members.push_back(member);
	}

	T get(const std::string& name) {
		if (!index.count(name)) {
			OtExcept("Member [%s] not in registry", name.c_str());
		}

		return members[index[name]];
	}

private:
	// registry of registered types
	std::vector<T> members;
	std::unordered_map<std::string, size_t> index;
};
