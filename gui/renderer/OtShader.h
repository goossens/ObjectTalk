//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtBgfxHelpers.h"


//
//	OtShader
//

class OtShader {
public:
	// states
	enum states {
		noDepth,
		wireframe,
		noCulling,
		cullFront,
		cullBack,
		blending
	};

	// constructors/destructor
	OtShader() = default;
	OtShader(const char* vertex, const char* fragment);

	// initialize uniform
	void initialize(const char* vertex, const char* fragment);

	// clear the resources
	void clear() { shader.clear(); }

	// see if shader is valid
	bool isValid() { return shader.isValid(); }

	// set parameters for the next run
	void setState(states state);
	void setTransform(const glm::mat4& transform);

	// run shader for specified view on GPU
	void submit(bgfx::ViewId view);

private:
	// uniform
	OtBgfxHandle<bgfx::ProgramHandle> shader;
};
