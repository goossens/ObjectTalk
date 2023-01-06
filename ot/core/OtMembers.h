//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


//
//	Forward references
//

class OtObjectClass;
typedef std::shared_ptr<OtObjectClass> OtObject;


//
//	OtMembers
//

class OtMembersClass;
typedef std::shared_ptr<OtMembersClass> OtMembers;

class OtMembersClass {
public:
	// access the members
	inline bool has(const std::string& name) { return members.count(name); }
	inline OtObject get(const std::string& name) { return members[name]; }
	inline void set(const std::string& name, OtObject member) { members[name] = member; }
	inline void unset(const std::string& name) { members.erase(name); }
	inline void unsetAll() { members.clear(); }

	std::vector<std::string> names() {
		std::vector<std::string> result;
		for (auto i : members) { result.push_back(i.first); }
		return result;
	}

	// create a new members hash table
	static OtMembers create() {
		return std::make_shared<OtMembersClass>();
	}

private:
	// the actual members
	std::unordered_map<std::string, OtObject> members;
};
