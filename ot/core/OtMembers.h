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
#include "OtSymbol.h"


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
	bool has(size_t symbol) {	return members.count(symbol); }
	OtObject& get(size_t symbol) { return members[symbol]; }
	void set(size_t symbol, OtObject member);
	void unset(size_t symbol) { members.erase(symbol); }
	void unsetAll() { members.clear(); }

	// get all member names
	void getMemberNames(std::vector<std::string_view>& names);

private:
	// the actual members
	std::unordered_map<size_t, OtObject> members;
};
