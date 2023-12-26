//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtNumbers.h"

#include "OtEllipticalCurve.h"


//
//	OtArcCurveClass
//

class OtArcCurveClass;
typedef std::shared_ptr<OtArcCurveClass> OtArcCurve;

class OtArcCurveClass : public OtEllipticalCurveClass {
public:
	// constructor
	inline OtArcCurveClass(const glm::vec2& c, float r, float sa=0.0f, float ea=std::numbers::pi2, bool clk=false) :
		OtEllipticalCurveClass(c, r, r, sa, ea, clk, 0.0f) {}
};
