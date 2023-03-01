//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtMembers.h"
#include "OtObject.h"


//
//	OtMembers::has
//

bool OtMembers::has(size_t selector) {
	return members.count(selector);
}


//
//	OtMembers::get
//

OtObject OtMembers::get(size_t selector) {
	return members[selector];
}


//
//	OtMembers::set
//

void OtMembers::set(size_t selector, OtObject member) {
	members[selector] = member;
}


//
//	OtMembers::unset
//

void OtMembers::unset(size_t selector) {
	members.erase(selector);
}


//
//	OtMembers::unsetAll
//

void OtMembers::unsetAll() {
	members.clear();
}


//
//	OtMembers::getMemberNames
//

std::vector<std::string> OtMembers::getMemberNames() {
	auto selector = OtSelector::instance();
	std::vector<std::string> result;

	for (auto i : members) {
		result.push_back(selector->get(i.first));
	}

	return result;
}
