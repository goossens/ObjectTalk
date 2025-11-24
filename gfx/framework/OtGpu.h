//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <atomic>

#include "SDL3/SDL.h"

#include "OtSingleton.h"


//
//	OtGpu
//

class OtGpu : public OtSingleton<OtGpu> {
public:
	// manage GPU lifecycle
	void init(SDL_Window* win, int w, int h);
	void release();

	// set a new window size
	void setWindowSize(int w, int h);

	// manage frames
	void startFrame();
	void endFrame();
	void flushAndRestartFrame();

	// rendering properties
	SDL_Window* window;
	SDL_GPUDevice* device;
	SDL_GPUCommandBuffer* copyCommandBuffer;
	SDL_GPUCommandBuffer* pipelineCommandBuffer;
	SDL_GPUTexture* swapchainTexture;
	int width;
	int height;

	// dummy textures
	SDL_GPUTexture* transparentDummyTexture;
	SDL_GPUTexture* blackDummyTexture;
	SDL_GPUTexture* whiteDummyTexture;
	SDL_GPUTexture* dummyCubeMap;

	// statistics
	std::atomic<int> computePasses = 0;
	std::atomic<int> computePipelines = 0;
	std::atomic<int> renderPasses = 0;
	std::atomic<int> renderPipelines = 0;
	std::atomic<int> renderShaders = 0;
	std::atomic<int> vertexBuffers = 0;
	std::atomic<int> indexBuffers = 0;
	std::atomic<int> frameBuffers = 0;
	std::atomic<int> gBuffers = 0;
	std::atomic<int> textures = 0;
	std::atomic<int> samplers = 0;
	std::atomic<int> cubemaps = 0;

	private:
	// support functions
	SDL_GPUTexture* createDummyTexture(SDL_GPUCopyPass* pass, SDL_GPUTransferBuffer* buffer, SDL_Color pixel);
	SDL_GPUTexture* createDummyCubeMap();

	void acquireCommandBuffers();
	void executeCommandBuffer();
};
