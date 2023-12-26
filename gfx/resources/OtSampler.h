//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtBgfxHandle.h"

#include "OtCubeMap.h"
#include "OtTexture.h"


//
//	OtSampler
//

class OtSampler {
public:
	// flags
	static constexpr uint32_t defaultSampling = UINT32_MAX;
	static constexpr uint32_t linearSampling = BGFX_SAMPLER_NONE;
	static constexpr uint32_t pointSampling = BGFX_SAMPLER_POINT;
	static constexpr uint32_t anisotropicSampling = BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC;
	static constexpr uint32_t repeatSampling = BGFX_SAMPLER_NONE;
	static constexpr uint32_t clampSampling = BGFX_SAMPLER_UVW_CLAMP;
	static constexpr uint32_t mirrorSampling = BGFX_SAMPLER_UVW_MIRROR;

	// constructor/destructor
	OtSampler() = default;
	inline OtSampler(const char* n, uint32_t f=defaultSampling) : name(n), flags(f) {}

	// initialize sampler
	void initialize(const char* name, uint32_t flags=defaultSampling);

	// clear the resources
	void clear();

	// (re)set/get the flags
	inline void resetFlags() { flags = defaultSampling; }
	inline void setFlags(uint32_t f) { flags = f; }
	inline uint32_t getFlags() { return flags; }

	// see if sampler is valid
	inline bool isValid() { return uniform.isValid(); }

	// bind texture to sampler and submit to GPU
	void submit(int unit, const char* name=nullptr); // bind dummy texture
	void submit(int unit, OtTexture& texture, const char* name=nullptr);
	void submit(int unit, bgfx::TextureHandle texture, const char* name=nullptr);
	void submit(int unit, OtCubeMap& cubemap, const char* name=nullptr);

private:
	// private method to create the uniform
	void createUniform();

	// properties
	std::string name;
	uint32_t flags = defaultSampling;

	// uniform
	OtBgfxHandle<bgfx::UniformHandle> uniform;
};
