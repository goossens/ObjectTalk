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

#include "OtSelector.h"


//
//	Forward references
//

class OtObjectClass;
typedef std::shared_ptr<OtObjectClass> OtObject;


//
//	OtMembers
//

class OtMembers {
public:
	// access the members
	inline bool has(size_t selector) { return members.count(selector); }
	inline OtObject get(size_t selector) { return members[selector]; }
	inline void set(size_t selector, OtObject member) { members[selector] = member; }
	inline void unset(size_t selector) { members.erase(selector); }

	inline void unsetAll() { members.clear(); }

	std::vector<std::string> getMemberNames() {
		auto selector = OtSelector::instance();
		std::vector<std::string> result;

		for (auto i : members) {
			result.push_back(selector->get(i.first));
		}

		return result;
	}

private:
	// the actual members
	std::unordered_map<size_t, OtObject> members;
};
