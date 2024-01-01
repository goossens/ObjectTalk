//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include <string>
#include <unordered_map>
#include <utility>

#include "OtException.h"


//
//	OtRegistry
//

template<class T>
class OtRegistry {
public:
	// add a new member to the registry
	void set(const std::string& name, T member) {
		if (registry.count(name)) {
			OtError("Member [%s] already in registry", name.c_str());
		}

		registry[name] = member;
	}

	// get member from registry
	T get(const std::string& name) {
		if (!registry.count(name)) {
			OtError("Member [%s] not in registry", name.c_str());
		}

		return registry[name];
	}

	// get reference to registry member
	T& at(const std::string& name) {
		if (!registry.count(name)) {
			OtError("Member [%s] not in registry", name.c_str());
		}

		return registry.at(name);
	}

	// see if registry has specified member
	bool has(const std::string& name) {
		return registry.count(name) != 0;
	}

	// iterate through registry members
	void iterate(std::function<void(const std::string&, T&)> callback) {
		for (std::pair<std::string, T> entry : registry) {
			callback(entry.first, entry.second);
		}
	}

	void iterateKeys(std::function<void(const std::string&)> callback) {
		for (std::pair<std::string, T> entry : registry) {
			callback(entry.first);
		}
	}

	void iterateValues(std::function<void(T&)> callback) {
		for (std::pair<std::string, T> entry : registry) {
			callback(entry.second);
		}
	}

	// get size of registry
	size_t size() {
		return registry.size();
	}

	// clear the registry
	void clear() {
		registry.clear();
	}

private:
	// registry member
	std::unordered_map<std::string, T> registry;
};
