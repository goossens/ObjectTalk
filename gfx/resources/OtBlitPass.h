//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "SDL3/SDL_gpu.h"

#include "OtGpu.h"
#include "OtTexture.h"


//
//	OtBlitPass
//

class OtBlitPass {
public:
	// blit entire texture to other texture
	static void blit(OtTexture& src, OtTexture& dst) {
		SDL_GPUBlitInfo info{};
		info.source.texture = src.getTexture();
		info.source.w = static_cast<Uint32>(src.getWidth());
		info.source.h = static_cast<Uint32>(src.getHeight());

		info.destination.texture = dst.getTexture();
		info.destination.w = static_cast<Uint32>(dst.getWidth());
		info.destination.h = static_cast<Uint32>(dst.getHeight());

		SDL_BlitGPUTexture(OtGpu::instance().pipelineCommandBuffer, &info);
	}
};
