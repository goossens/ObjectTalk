//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>


//
//	OtWorldLayer
//

class OtWorldLayer {
public:
	// constructor
	OtWorldLayer(const std::string& name, deform=false) : name(name), deform(deform) {}

private:
	// properties
	std::string name;
	bool deform;
};
