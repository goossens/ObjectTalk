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
//	OtPosterize
//

class OtPosterize : public OtFilter {
public:
	// set properties
	inline void setLevels(int l) { levels = l; }

private:
	// execute filter
	void execute(OtPass& pass) override;

	// properties
	int levels = 10;

	// GPU assets
	OtUniformVec4 uniform = OtUniformVec4("u_posterize", 1);
	OtShaderProgram program = OtShaderProgram("OtFilterVS", "OtPosterizeFS");
};
