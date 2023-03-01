//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

#include "OtObjectPointer.h"
#include "OtSelector.h"


//
//	Forward references
//

class OtObjectClass;
using OtObject = OtObjectPointer<OtObjectClass>;


//
//	OtMembers
//

class OtMembers {
public:
	// access the members
	bool has(size_t selector);
	OtObject get(size_t selector);
	void set(size_t selector, OtObject member);
	void unset(size_t selector);
	void unsetAll();

	// get all member names
	std::vector<std::string> getMemberNames();

private:
	// the actual members
	std::unordered_map<size_t, OtObject> members;
};
