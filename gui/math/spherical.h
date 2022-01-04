//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "guimath.h"


//
//	OtSpherical
//

class OtSphericalClass;
typedef std::shared_ptr<OtSphericalClass> OtSpherical;

class OtSphericalClass : public OtMathClass {
public:
	// constructors
	OtSphericalClass() {}
	OtSphericalClass(float r, float p, float t) : radius(r), phi(p), theta(t) {}

	// initialize plane
	OtObject init(size_t count, OtObject* parameters);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtSpherical create();

private:
	// spherical properties: radius polar angle and azimuthal angle
	float radius = 1.0;
	float phi = 0.0;
	float theta = 0.0;
};
