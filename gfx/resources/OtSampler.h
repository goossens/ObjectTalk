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
	// constructor/destructor
	OtSampler() = default;
	inline OtSampler(const char* n, uint64_t f=OtTexture::defaultSampling) : name(n), flags(f) {}

	// initialize sampler
	void initialize(const char* name, uint64_t flags=OtTexture::defaultSampling);

	// clear the resources
	void clear();

	// (re)set/get the flags
	inline void resetFlags() { flags = OtTexture::defaultSampling; }
	inline void setFlags(uint64_t f) { flags = f; }
	inline uint64_t getFlags() { return flags; }

	// see if sampler is valid
	inline bool isValid() { return uniform.isValid(); }

	// bind texture/cubemap to sampler and submit to GPU
	void submit(int unit, OtTexture& texture, const char* name=nullptr);
	void submit(int unit, bgfx::TextureHandle texture, const char* name=nullptr);
	void submit(int unit, OtCubeMap& cubemap, const char* name=nullptr);

	void submitDummyTexture(int unit, const char* name=nullptr); // bind dummy texture
	void submitDummyCubeMap(int unit, const char* name=nullptr); // bind dummy cubemap

private:
	// private method to create the uniform
	void createUniform();

	// properties
	std::string name;
	uint64_t flags = OtTexture::defaultSampling;

	// uniform
	OtBgfxHandle<bgfx::UniformHandle> uniform;
};
