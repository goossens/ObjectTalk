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
#include <string>

#include "OtBgfxHandle.h"
#include "OtTexture.h"


//
//	Forward declaration
//

class OtCubeMap;


//
//	OtSampler
//

class OtSampler {
public:
	// sampler flags
	static constexpr uint64_t linearSampling = BGFX_SAMPLER_NONE;
	static constexpr uint64_t pointSampling = BGFX_SAMPLER_POINT;
	static constexpr uint64_t anisotropicSampling = BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC;
	static constexpr uint64_t repeatSampling = BGFX_SAMPLER_NONE;
	static constexpr uint64_t clampSampling = BGFX_SAMPLER_UVW_CLAMP;
	static constexpr uint64_t mirrorSampling = BGFX_SAMPLER_UVW_MIRROR;
	static constexpr uint64_t defaultSampling = linearSampling | repeatSampling;

	// constructor
	inline OtSampler(const char* n, uint64_t f=defaultSampling) : name(n), flags(f) {}

	// clear the resources
	void clear();

	// (re)set the flags
	void setFlags(uint64_t f);

	// see if sampler is valid
	inline bool isValid() { return uniform.isValid(); }

	// bind texture/cubemap to sampler and submit to GPU
	void submit(int unit, OtTexture& texture);
	void submit(int unit, bgfx::TextureHandle texture);
	void submit(int unit, OtCubeMap& cubemap);

	void submitDummyTexture(int unit); // bind dummy texture
	void submitDummyCubeMap(int unit); // bind dummy cubemap

private:
	// private method to create the uniform
	void createUniform();

	// properties
	std::string name;
	uint64_t flags;

	// uniform
	OtBgfxHandle<bgfx::UniformHandle> uniform;
};
