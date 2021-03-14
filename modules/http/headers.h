//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <unordered_map>

#include "ot/text.h"


//
//	Case insensitive headers map
//

class OtHttpHeadersComparator {
public:
	bool operator()(const std::string& s1, const std::string& s2) const {
		return OtTextCaseCmp(s1, s2) == 0;
	}
};

class OtHttpHeaders : public std::unordered_multimap<std::string, std::string, std::hash<std::string>, OtHttpHeadersComparator> {
public:
	bool has(const std::string& name) {
		return find(name) != end();
	}

	const std::string get(const std::string& header) {
		if (has(header)) {
			auto range = equal_range(header);

			if (range.first == range.second) {
				return range.first->second;

			} else {
				std::string result;

				for (auto i = range.first; i != range.second; i++) {
					if (i != range.first) {
						result.append("; ");
					}

					result.append(i->second);
				}

				return result;
			}

		} else {
			return "";
		}
	}
};
