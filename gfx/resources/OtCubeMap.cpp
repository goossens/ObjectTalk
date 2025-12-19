//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>

#include "nlohmann/json.hpp"
#include "stb_image.h"

#include "OtLog.h"
#include "OtPath.h"
#include "OtText.h"

#include "OtAsset.h"
#include "OtCubeMap.h"
#include "OtRenderPass.h"
#include "OtTexture.h"
#include "OtVertex.h"

#include "OtFullScreenVert.h"
#include "OtHdrReprojectFrag.h"


//
//	OtCubeMap::create
//

void OtCubeMap::create(Format f, int s, bool m) {
	// save settings
	format = f;
	size = s;
	mip = m;

	// create new cubemap
	SDL_GPUTextureCreateInfo info{};
	info.type = SDL_GPU_TEXTURETYPE_CUBE;
	info.format = static_cast<SDL_GPUTextureFormat>(format);
	info.usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_WRITE;
	info.width = static_cast<Uint32>(size);
	info.height = static_cast<Uint32>(size);
	info.layer_count_or_depth = 6;
	info.num_levels = static_cast<Uint32>(mip ? getMipLevels() : 1);
	info.sample_count = SDL_GPU_SAMPLECOUNT_1;

	auto sdlCubemap = SDL_CreateGPUTexture(OtGpu::instance().device, &info);

	if (!sdlCubemap) {
		OtLogFatal("Error in SDL_CreateGPUTexture: {}", SDL_GetError());
	}

	if (mip) {
		SDL_GenerateMipmapsForGPUTexture(OtGpu::instance().copyCommandBuffer, sdlCubemap);
	}

	assign(sdlCubemap);
	incrementVersion();
}


//
//	OtCubeMap::load
//

void OtCubeMap::load(const std::string& path, bool async) {
	auto ext = OtPath::getExtension(path);

	if (ext == ".json") {
		loadJSON(path, async);

	} else {
		loadHdrImage(path, async);
	}
}


//
//	OtCubeMap::clear
//

void OtCubeMap::clear() {
	cubemap = nullptr;
	size = 1;
	mip = false;
}


//
//	OtCubeMap::assign
//

void OtCubeMap::assign(SDL_GPUTexture* newTexture) {
	cubemap = std::shared_ptr<SDL_GPUTexture>(
		newTexture,
		[](SDL_GPUTexture* oldTexture) {
			auto& gpu = OtGpu::instance();
			SDL_ReleaseGPUTexture(gpu.device, oldTexture);
			gpu.cubemaps--;
		});

	incrementVersion();
	OtGpu::instance().cubemaps++;
}

//
//	OtCubeMap::loadJSON
//

void OtCubeMap::loadJSON(const std::string& path, bool async) {
	// parse json
	std::string text;
	OtText::load(path, text);

	auto basedir = OtPath::getParent(path);
	auto data = nlohmann::json::parse(text);
	size = data.value("size", 0);

	if (!size) {
		OtLogError("Invalid size in CubeMap specification [{}]", path);
	}

	bytesPerImage = static_cast<size_t>(size * size * 4);
	imageData = std::make_unique<std::byte[]>(bytesPerImage * 6);

	std::array<std::string, 6> paths = {
		OtAssetDeserialize(&data, "posx", &basedir),
		OtAssetDeserialize(&data, "negx", &basedir),
		OtAssetDeserialize(&data, "posy", &basedir),
		OtAssetDeserialize(&data, "negy", &basedir),
		OtAssetDeserialize(&data, "posz", &basedir),
		OtAssetDeserialize(&data, "negz", &basedir)
	};

	// load images
	for (size_t i = 0; i < 6; i++) {
		// sanity check
		if (paths[i].empty()) {
			OtLogError("Incomplete CubeMap specification [{}]", path);
		}

		// load image
		int w, h, n;
		auto pixels = stbi_load(paths[i].c_str(), &w, &h, &n, 4);

		if (!pixels) {
			OtLogError("Can't open image in [{}]", path);
		}

		if (w != size) {
			OtLogFatal("Image [{}] has incorrect width [{}]. Should be [{}]", paths[i], w, size);

		} else if (h != size) {
			OtLogFatal("Image [{}] has incorrect height [{}]. Should be [{}]", paths[i], h, size);
		}

		// copy pixels
		std::memcpy(imageData.get() + bytesPerImage * i, pixels, bytesPerImage);
		stbi_image_free(pixels);
	}

	if (async) {
		// schedule a task to upload images to texture
		// we can't do that here as loading is done in a separate thread
		// the callback below will be called in the main thread
		asyncHandle = new uv_async_t;
		asyncHandle->data = this;

		auto status = uv_async_init(uv_default_loop(), asyncHandle, [](uv_async_t* handle){
			auto cubemap = (OtCubeMap*) handle->data;
			cubemap->createCubemapFromSides();

			// cleanup
			uv_close((uv_handle_t*) cubemap->asyncHandle, [](uv_handle_t* handle) {
				auto cubemap = (OtCubeMap*) handle->data;
				delete (uv_fs_event_t*) handle;
				cubemap->asyncHandle = nullptr;
			});
		});

		UV_CHECK_ERROR("uv_async_init", status);

		status = uv_async_send(asyncHandle);
		UV_CHECK_ERROR("uv_async_send", status);

	} else {
		createCubemapFromSides();
	}
}


//
//	OtCubeMap::loadHdrImage
//

void OtCubeMap::loadHdrImage(const std::string& path, bool async) {
	// load image
	int w, h, n;
	auto pixels = stbi_loadf(path.c_str(), &w, &h, &n, 4);

	if (!pixels) {
		OtLogError("Can't open image in [{}]", path);
	}

	asyncImage = std::make_unique<OtImage>(w, h, OtImage::Format::rgba32, pixels);
	stbi_image_free(pixels);

	size = 1024;

	if (async) {
		// schedule a task to upload and convert image to texture
		// we can't do that here as loading is done in a separate thread
		// the callback below will be called in the main thread
		asyncHandle = new uv_async_t;
		asyncHandle->data = this;

		auto status = uv_async_init(uv_default_loop(), asyncHandle, [](uv_async_t* handle){
			auto cubemap = (OtCubeMap*) handle->data;
			cubemap->createCubemapFromHDR();

			// cleanup
			uv_close((uv_handle_t*) cubemap->asyncHandle, [](uv_handle_t* handle) {
				auto cubemap = (OtCubeMap*) handle->data;
				delete (uv_fs_event_t*) handle;
				cubemap->asyncHandle = nullptr;
				cubemap->asyncImage = nullptr;
			});
		});

		UV_CHECK_ERROR("uv_async_init", status);

		status = uv_async_send(asyncHandle);
		UV_CHECK_ERROR("uv_async_send", status);

	} else {
		createCubemapFromHDR();
	}
}


//
//	OtCubeMap::createCubemapFromSides
//

void OtCubeMap::createCubemapFromSides() {
	// create new cubemap
	create(Format::rgba8, size, false);

	// create a transfer buffer
	SDL_GPUTransferBufferCreateInfo bufferInfo{};
	bufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
	bufferInfo.size = static_cast<Uint32>(bytesPerImage * 6);

	auto& gpu = OtGpu::instance();
	SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(gpu.device, &bufferInfo);

	if (!transferBuffer) {
		OtLogFatal("Error in SDL_CreateGPUTransferBuffer: {}", SDL_GetError());
	}

	// put images in transfer buffer
	void* data = SDL_MapGPUTransferBuffer(gpu.device, transferBuffer, false);
	std::memcpy(data, imageData.get(), bytesPerImage * 6);
	SDL_UnmapGPUTransferBuffer(gpu.device, transferBuffer);

	// upload images to GPU
	SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(gpu.copyCommandBuffer);

	for (size_t i = 0; i < 6; i++) {
		SDL_GPUTextureTransferInfo transferInfo{};
		transferInfo.transfer_buffer = transferBuffer;
		transferInfo.offset = static_cast<Uint32>(bytesPerImage * i);

		SDL_GPUTextureRegion region{};
		region.texture = cubemap.get();
		region.layer = static_cast<Uint32>(i);
		region.w = static_cast<Uint32>(size);
		region.h = static_cast<Uint32>(size);
		region.d = 1;

		SDL_UploadToGPUTexture(copyPass, &transferInfo, &region, false);
	}

	SDL_EndGPUCopyPass(copyPass);
	SDL_ReleaseGPUTransferBuffer(gpu.device, transferBuffer);

	// housekeeping
	incrementVersion();
	imageData = nullptr;
}


//
//	OtCubeMap::createCubemapFromHDR
//

void OtCubeMap::createCubemapFromHDR() {
	// upload image to GPU texture
	OtTexture inputTexture;
	inputTexture.load(*asyncImage);

	// create a cubemap texture
	create(Format::rgba16, size, true);

	// setup the rendering pipeline (if required)
	if (!hdrPipeline.isValid()) {
		hdrPipeline.setShaders(OtFullScreenVert, sizeof(OtFullScreenVert), OtHdrReprojectFrag, sizeof(OtHdrReprojectFrag));
		hdrPipeline.setRenderTargetType(OtRenderPipeline::RenderTargetType::rgba16);
	}

	// run render passes
	for (size_t i = 0; i < 6; i++) {
		struct Uniforms {
			int32_t side;
		} uniforms {
			static_cast<int32_t>(i)
		};

		OtRenderPass pass;
		pass.start2(*this, i);
		pass.bindFragmentSampler(0, hdrSampler, inputTexture);
		pass.bindFragmentUniforms(0, &uniforms, sizeof(uniforms));
		pass.bindPipeline(hdrPipeline);
		pass.render(3);
		pass.end();
	}
}
