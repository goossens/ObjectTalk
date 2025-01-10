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
//	OtNormalMapper
//

class OtNormalMapper : public OtFilter {
public:
	// determine if resulting normalmap includes height
	inline void includeHeight(bool flag) { includeHeightFlag = flag; }

	// set strength of normals
	inline void setStrength(float strength) { normalStrength = strength; }

private:
	// execute filter
	void execute(OtPass& pass) override;

	// shader resources
	OtShaderProgram program{"OtFilterVS", "OtNormalMapperFS"};
	OtUniformVec4 uniform = OtUniformVec4{"u_normalMapper", 1};
	bool includeHeightFlag = false;
	float normalStrength = 10.0f;
};
