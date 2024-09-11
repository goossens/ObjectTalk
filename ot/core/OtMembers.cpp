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

void OtMembers::set(size_t symbol, OtObject member) {
	members[symbol] = member;
}


//
//	OtMembers::getMemberNames
//

void OtMembers::getMemberNames(std::vector<std::string_view>& names) {
	names.clear();
	auto symbolizer = OtSymbolizer::instance();

	for (auto i : members) {
		names.emplace_back(symbolizer->get(i.first));
	}
}
