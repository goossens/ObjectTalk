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
	inline void set(const std::string& name, T member) {
		if (registry.count(name)) {
			OtError("Member [{}] already in registry", name);
		}

		registry[name] = member;
	}

	// get member from registry
	inline T get(const std::string& name) {
		if (!registry.count(name)) {
			OtError("Member [{}] not in registry", name);
		}

		return registry[name];
	}

	// get reference to registry member
	inline T& at(const std::string& name) {
		if (!registry.count(name)) {
			OtError("Member [{}] not in registry", name);
		}

		return registry.at(name);
	}

	// see if registry has specified member
	inline bool has(const std::string& name) {
		return registry.count(name) != 0;
	}

	// iterate through registry members
	inline void iterate(std::function<void(const std::string&, T&)> callback) {
		for (std::pair<std::string, T> entry : registry) {
			callback(entry.first, entry.second);
		}
	}

	inline void iterateKeys(std::function<void(const std::string&)> callback) {
		for (std::pair<std::string, T> entry : registry) {
			callback(entry.first);
		}
	}

	inline void iterateValues(std::function<void(T&)> callback) {
		for (std::pair<std::string, T> entry : registry) {
			callback(entry.second);
		}
	}

	// get size of registry
	inline size_t size() {
		return registry.size();
	}

	// clear the registry
	inline void clear() {
		registry.clear();
	}

private:
	// registry member
	std::unordered_map<std::string, T> registry;
};
