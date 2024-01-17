//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
//	OtSeamlessTile
//

class OtSeamlessTile : public OtFilter {
private:
	// execute filter
	void execute(OtPass& pass) override;

	// shader resources
	OtShaderProgram program{"OtFilterVS", "OtSeamlessTileFS"};
};
