//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtMembers.h"
#include "OtObject.h"


//
//	OtMembers::set
//

void OtMembers::set(size_t selector, OtObject member) {
	members[selector] = member;
}


//
//	OtMembers::getMemberNames
//

void OtMembers::getMemberNames(std::vector<std::string_view>& names) {
	names.clear();
	auto selector = OtSelector::instance();

	for (auto i : members) {
		names.emplace_back(selector->get(i.first));
	}
}
