//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtFormat
//

template <typename... Args>
std::string OtFormat(const std::string &format, Args... args) {
	char output[1024];
	std::snprintf(output, 1024, format.c_str(), args...);
	return std::string(output);
}
