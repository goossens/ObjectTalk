//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cmath>

#include "OtComputePass.h"
#include "OtComputePipeline.h"
#include "OtTexture.h"


//
//	OtGenerator
//

class OtGenerator {
public:
	// destructor
	virtual inline ~OtGenerator() {}

	// clear GPU resources
	virtual inline void clear() {
		pipeline.clear();
	}

	// method to be overridden by derived classes
	virtual void configurePass(OtComputePass& pass) = 0;

	// let generator render to texture
	void render(OtTexture& texture) {
		OtComputePass pass;
		pass.addOutputTexture(texture);

		// ask derived class to configure the compute pass
		// e.g. create compute pipeline and/or set uniforms
		configurePass(pass);

		pass.execute(
			pipeline,
			static_cast<size_t>(std::ceil(texture.getWidth() / 16.0)),
			static_cast<size_t>(std::ceil(texture.getHeight() / 16.0)),
			1);
	}

protected:
	// the generator specific rendering pipeline (to be set by derived class)
	OtComputePipeline pipeline;
};
