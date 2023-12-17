//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFilter.h"
#include "OtShaderProgram.h"


//
//	OtBloom
//

class OtBloom : public OtFilter {
private:
	// execute filter
	void execute(OtPass& pass) override;

	// GPU assets
	OtShaderProgram program = OtShaderProgram("OtFilterVS", "OtBloomFS");
};
