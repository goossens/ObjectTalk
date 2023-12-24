//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtException.h"


//
//	OtException::serialize
//

std::string OtException::serialize() {
	auto data = nlohmann::json::object();
	data["module"] = module;
	data["lineNumber"] = lineNumber;
	data["start"] = start;
	data["end"] = end;
	data["shortMessage"] = shortMessage;
	data["longMessage"] = longMessage;
	return data.dump();
}


//
//	OtException::deserialize
//

void OtException::deserialize(const std::string& string) {
	auto data = nlohmann::json::parse(string);
	module = data.value("module", "unknown");
	lineNumber = data.value("lineNumber", 0);
	start = data.value("start", 0);
	end = data.value("end", 0);
	shortMessage = data.value("shortMessage", "");
	longMessage = data.value("longMessage", "");
}
