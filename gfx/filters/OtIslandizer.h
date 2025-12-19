//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>

#include "OtFilter.h"
#include "OtShaders.h"


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
	inline void setDistanceFunction(DistanceFunction value) { distanceFunction = value; }

	// configure the compute pass
	void configurePass(OtComputePass& pass) override {
		// initialize pipeline (if required)
		if (!pipeline.isValid()) {
			pipeline.setShader(OtIslandizerComp, OtIslandizerCompSize);
		}

		// set uniforms
		struct Uniforms {
			int32_t distanceFunction;
		} uniforms {
			static_cast<int32_t>(distanceFunction)
		};

		pass.addUniforms(&uniforms, sizeof(uniforms));
	}

private:
	// properties
	DistanceFunction distanceFunction = DistanceFunction::squareBump;
};
