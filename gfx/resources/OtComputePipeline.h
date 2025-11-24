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
#include <memory>

#include "OtLog.h"

#include "SDL3/SDL_gpu.h"
#include "SDL3_shadercross/SDL_shadercross.h"

#include "OtGpu.h"


//
//	OtComputePipeline
//

class OtComputePipeline {
public:
	// constructor
	OtComputePipeline() = default;
	OtComputePipeline(const uint32_t* code, size_t size) : computeShaderCode(code), computeShaderSize(size) {}

	// associate shader with pipeline
	void setShader(const uint32_t* code, size_t size);

	// clear the object
	void clear();

	// see if pipeline is valid
	inline bool isValid() { return pipeline != nullptr; }

private:
	// shader definition
	const uint32_t* computeShaderCode = nullptr;
	size_t computeShaderSize = 0;

	// the GPU resource
	std::shared_ptr<SDL_GPUComputePipeline> pipeline;

	// memory manage SDL resource
	void assign(SDL_GPUComputePipeline* newPipeline);

	// get the raw pipeline object
	friend class OtComputePass;
	SDL_GPUComputePipeline* getPipeline();
};
