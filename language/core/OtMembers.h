//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
	inline bool has(OtID id) { return members.count(id); }
	inline OtObject get(OtID id) { return members[id]; }
	inline void set(OtID id, OtObject member) { members[id] = member; }
	inline void unset(OtID id) { members.erase(id); }
	inline void unsetAll() { members.clear(); }

	// iterate through the members
	inline void each(std::function<void(OtID, OtObject object)> callback) {
		for (auto i : members) {
			callback(i.first, i.second);
		}
	}

	inline void eachID(std::function<void(OtID)> callback) {
		for (auto i : members) {
			callback(i.first);
		}
	}

private:
	// the actual members
	std::unordered_map<OtID, OtObject> members;
};
