//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtHash.h"
#include "OtLog.h"

#include "OtFrameworkAtExit.h"
#include "OtGpu.h"
#include "OtRenderShader.h"


//
//	OtRenderShader::OtRenderShader
//

OtRenderShader::OtRenderShader(const uint32_t* code, size_t size, Stage stage) {
	// initialize cache (if required)
	if (!cacheInitialized) {
		// we need to clear the cache when the framework exits
		// waiting until the cache is destructed doesn't work
		// since the GPU context is no longer available at that point
		OtFrameworkAtExit::add([]() {
			for (auto& [hash, shader]: shaders) {
				SDL_ReleaseGPUShader(OtGpu::instance().device, shader);
			}

			shaders.clear();
		});

		cacheInitialized = true;
	}

	// see if shader has already been compiled
	auto hash = OtHash::generate(code, size, stage);

	if (shaders.find(hash) == shaders.end()) {
		// shader is not in cache so we need to cross compile it

		// figure out shader metadata
		SDL_ShaderCross_GraphicsShaderMetadata* metadata = SDL_ShaderCross_ReflectGraphicsSPIRV((Uint8*) code, size, 0);

		if (metadata == nullptr) {
			OtLogFatal("Error in SDL_ShaderCross_ReflectGraphicsSPIRV: {}", SDL_GetError());
		}

		// cross compile to the appropriate shader format and create a shader object
		SDL_ShaderCross_SPIRV_Info info{};
		info.bytecode = (Uint8*) code;
		info.bytecode_size = size;
		info.entrypoint = "main";
		info.shader_stage = static_cast<SDL_ShaderCross_ShaderStage>(stage);
		shader = SDL_ShaderCross_CompileGraphicsShaderFromSPIRV(OtGpu::instance().device, &info, &metadata->resource_info, 0);

		if (shader == nullptr) {
			OtLogFatal("Error in SDL_ShaderCross_CompileGraphicsShaderFromSPIRV: {}", SDL_GetError());
		}

		// store shader in cache
		shaders[hash] = shader;
		OtGpu::instance().renderShaders++;

		// cleanup
		SDL_free(metadata);

	} else {
		// shader was already compiled, get it from the cache
		shader = shaders[hash];
	}
}
