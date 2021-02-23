//	ObjectTalk Scripting Language
//	Copyright 1993-2021 Johan A. Goossens
//
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.


//
//	Case insensitive headers map
//

class OtHttpHeadersComparator {
public:
    bool operator()(const std::string& s1, const std::string& s2) const {
		return std::lexicographical_compare(
			s1.begin(),
			s1.end(),
			s2.begin(),
			s2.end(),
			[](unsigned char c1, unsigned char c2) {
				return ::tolower(c1) < ::tolower(c2);
			});
    }
};

class OtHttpHeaders : public std::multimap<std::string, std::string, OtHttpHeadersComparator> {
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
