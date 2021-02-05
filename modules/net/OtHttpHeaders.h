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
    bool operator()(const std::string& a, const std::string& b) const {
		return ::strcasecmp(a.c_str(), b.c_str()) < 0;
    }
};

class OtHttpHeaders : public std::map<std::string, std::string, OtHttpHeadersComparator> {
public:
	bool has(const std::string& name) {
		return find(name) != end();
	}
};
