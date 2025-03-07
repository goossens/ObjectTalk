//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFilter.h"
#include "OtShaderProgram.h"
#include "OtUniformVec4.h"


//
//	OtPixelate
//

class OtPixelate : public OtFilter {
public:
	// set properties
	inline void setSize(int s) { size = s; }

private:
	// execute filter
	void execute(OtPass& pass) override;

	// properties
	int size = 5;

	// GPU assets
	OtUniformVec4 uniform = OtUniformVec4("u_pixelate", 1);
	OtShaderProgram program = OtShaderProgram("OtFilterVS", "OtPixelateFS");
};
