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
#include <string_view>
#include <unordered_map>
#include <vector>

#include "OtObjectPointer.h"
#include "OtIdentifier.h"


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
	bool has(size_t id) {	return members.count(id); }
	OtObject& get(size_t id) { return members[id]; }
	void set(size_t id, OtObject member);
	void unset(size_t id) { members.erase(id); }
	void unsetAll() { members.clear(); }

	// get all member names
	void getMemberNames(std::vector<std::string_view>& names);

private:
	// the actual members
	std::unordered_map<size_t, OtObject> members;
};
