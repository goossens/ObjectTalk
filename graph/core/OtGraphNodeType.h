//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include <string>

#include "OtGraphNode.h"


//
//	OtGraphNodeType
//

class OtGraphNodeType {
public:
	// constructor
	inline OtGraphNodeType(const char* n, std::function<OtGraphNode()> c) : name(n), constructor(c) {}

	// compare types
	inline bool operator < (const OtGraphNodeType& type) const {
		return name < type.name;
	}

	// properties
	std::string name;
	std::function<OtGraphNode()> constructor;
};
