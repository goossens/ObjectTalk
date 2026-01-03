//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <unordered_map>

#include "SDL3/SDL.h"
#include "SDL3_shadercross/SDL_shadercross.h"


//
//	OtRenderShader
//

class OtRenderShader {
public:
	// shader stages
	enum class Stage {
		vertex = SDL_SHADERCROSS_SHADERSTAGE_VERTEX,
		fragment = SDL_SHADERCROSS_SHADERSTAGE_FRAGMENT
	};

	// constructor
	OtRenderShader(const uint32_t* code, size_t size, Stage stage);

private:
	// the GPU resource
	SDL_GPUShader* shader = nullptr;

	// get the raw shader object
	friend class OtRenderPipeline;
	inline SDL_GPUShader* getShader() { return shader; }

	// shader cache
	static inline bool cacheInitialized = false;
	static inline std::unordered_map<size_t, SDL_GPUShader*> shaders;
};
