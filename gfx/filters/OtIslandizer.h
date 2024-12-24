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
#include "OtUniformVec4.h"


//
//	OtIslandizer
//

class OtIslandizer : public OtFilter {
public:
	// distance functions
	enum class DistanceFunction {
		squareBump,
		euclidean,
		euclidean2,
		hyperboloid,
		diagonal };

	static constexpr const char* distanceFunctions[] = {
		"Square Bump Distance",
		"Euclidean Distance",
		"Euclidean2 Distance",
		"Hyperboloid Distance",
		"Diagonal Distance" };

	static constexpr size_t distanceFunctionCount = sizeof(distanceFunctions) / sizeof(*distanceFunctions);

	// set properties
	inline void setDistanceFunction(DistanceFunction df) { distanceFunction = df; }

private:
	// execute filter
	void execute(OtPass& pass) override;

	// shader resources
	OtShaderProgram program{"OtFilterVS", "OtIslandizerFS"};
	OtUniformVec4 uniform{"u_islander", 1};

	// properties
	DistanceFunction distanceFunction = DistanceFunction::squareBump;
};
