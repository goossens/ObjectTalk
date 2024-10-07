//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstddef>
#include <functional>
#include <unordered_map>
#include <vector>

#include "OtIdentifier.h"
#include "OtObjectPointer.h"


//
//	Forward declarations
//

class OtObjectClass;
using OtObject = OtObjectPointer<OtObjectClass>;


//
//	OtMembers
//

class OtMembers {
public:
	// access the members
	inline bool has(size_t id) { return members.count(id); }
	inline OtObject& get(size_t id) { return members[id]; }
	inline void set(size_t id, OtObject member) { members[id] = member; }
	inline void unset(size_t id) { members.erase(id); }
	inline void unsetAll() { members.clear(); }

	// iterate through the members
	inline void each(std::function<void(size_t, OtObject object)> callback) {
		for (auto i : members) {
			callback(i.first, i.second);
		}
	}

	inline void eachID(std::function<void(size_t)> callback) {
		for (auto i : members) {
			callback(i.first);
		}
	}

private:
	// the actual members
	std::unordered_map<size_t, OtObject> members;
};
