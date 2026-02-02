//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtFaust.h"


//
//	OtFaust::serialize
//

void OtFaust::serialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) {
	iterateParameters([data](const char* name, float* variable, float) {
		(*data)[name] = *variable;
	});
}


//
//	OtFaust::deserialize
//

void OtFaust::deserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) {
	iterateParameters([data](const char* name, float* variable, float init) {
		*variable = data->value(name, init);
	});
}
