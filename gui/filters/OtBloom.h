//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFilter.h"
#include "OtShader.h"
#include "OtUniformVec4.h"


//
//	OtBloomClass
//

class OtBloomClass;
typedef std::shared_ptr<OtBloomClass> OtBloom;

class OtBloomClass : public OtFilterClass {
public:
	// execute filter
	void execute(OtPass& pass, int w, int h) override;

private:
	// GPU assets
	OtUniformVec4 uniform = OtUniformVec4("u_bloom", 1);
	OtShader shader = OtShader("OtBloomVS", "OtBloomFS");
};
