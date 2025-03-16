//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtImage.h"
#include "OtFrameworkAtExit.h"
#include "OtTexture.h"


//
//	OtTexture::clear
//

void OtTexture::clear() {
	texture.clear();
	width = 1;
	height = 1;
	format = noTexture;
	incrementVersion();
}


//
//	OtTexture::create
//

void OtTexture::create(int w, int h, int f, uint64_t flags) {
	width = w;
	height = h;
	format = f;
	incrementVersion();

	texture = bgfx::createTexture2D(
		static_cast<uint16_t>(width),
		static_cast<uint16_t>(height),
		false,
		1,
		bgfx::TextureFormat::Enum(format),
		flags);
}


//
//	createRegularTexture
//

static bgfx::TextureHandle createRegularTexture(bimg::ImageContainer* image) {
	return bgfx::createTexture2D(
		static_cast<uint16_t>(image->m_width),
		static_cast<uint16_t>(image->m_height),
		false,
		image->m_numLayers,
		bgfx::TextureFormat::Enum(image->m_format),
		BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
		bgfx::copy(image->m_data, image->m_size));
}


//
//	createMipmapTexture
//

static bgfx::TextureHandle createMipmapTexture(bimg::ImageContainer* image) {
	// create a new empty texture
	bgfx::TextureHandle texture = bgfx::createTexture2D(
		static_cast<uint16_t>(image->m_width),
		static_cast<uint16_t>(image->m_height),
		true,
		image->m_numLayers,
		bgfx::TextureFormat::Enum(image->m_format));

	const bimg::ImageBlockInfo& blockInfo = getBlockInfo(image->m_format);
	const uint32_t blockWidth  = blockInfo.blockWidth;
	const uint32_t blockHeight = blockInfo.blockHeight;

	uint32_t w = image->m_width;
	uint32_t h = image->m_height;

	// process all mip levels
	for (auto lod = 0; lod < image->m_numMips; lod++) {
		w = std::max(blockWidth,  w);
		h = std::max(blockHeight, h);

		bimg::ImageMip mip;

		if (bimg::imageGetRawData(*image, 0, static_cast<uint8_t>(lod), image->m_data, image->m_size, mip)) {
			bgfx::updateTexture2D(
				texture,
				0,
				static_cast<uint8_t>(lod),
				0,
				0,
				static_cast<uint16_t>(w),
				static_cast<uint16_t>(h),
				bgfx::copy(mip.m_data, mip.m_size));
		}

		w  >>= 1;
		h >>= 1;
	}

	return texture;
}


//
//	OtTexture::loadFromImage
//

void OtTexture::loadFromImage(OtImage& image) {
	// get the image data
	bimg::ImageContainer* container = image.getContainer();

	// update properties
	width = container->m_width;
	height = container->m_height;
	format = container->m_format;
	incrementVersion();

	// create texture
	if (container->m_numMips > 1) {
		texture = createMipmapTexture(container);

	} else {
		texture = createRegularTexture(container);
	}
}


//
//	OtTexture::loadFromFile
//

void OtTexture::loadFromFile(const std::string& path, bool async) {
	if (async) {
		asyncImage = std::make_shared<OtImage>(path);

		// schedule a task to upload image to texture
		// we can't do that here as loading is done in a seperate thread
		// the callback below will be called in the main thread
		asyncHandle = new uv_async_t;
		asyncHandle->data = this;

		auto status = uv_async_init(uv_default_loop(), asyncHandle, [](uv_async_t* handle){
			// run the HDR to cubemap conversion
			auto texture = (OtTexture*) handle->data;
			texture->loadFromImage(*texture->asyncImage);
			texture->asyncImage = nullptr;

			// cleanup
			uv_close((uv_handle_t*) texture->asyncHandle, [](uv_handle_t* handle) {
				auto texture = (OtTexture*) handle->data;
				delete (uv_fs_event_t*) handle;
				texture->asyncHandle = nullptr;
			});
		});

		UV_CHECK_ERROR("uv_async_init", status);

		status = uv_async_send(asyncHandle);
		UV_CHECK_ERROR("uv_async_send", status);

	} else {
		// get the image
		OtImage image(path);
		loadFromImage(image);
	}
}


//
//	OtTexture::loadFromMemory
//

void OtTexture::loadFromMemory(int w, int h, int f, void* pixels) {
	// update properties
	width = w;
	height = h;
	format = f;
	incrementVersion();

	// determine image size
	int size = bimg::imageGetSize(
		nullptr,
		static_cast<uint16_t>(w),
		static_cast<uint16_t>(h),
		1,
		false,
		false,
		1,
		bimg::TextureFormat::Enum(format));

	// create texture
	texture = bgfx::createTexture2D(
		static_cast<uint16_t>(w),
		static_cast<uint16_t>(h),
		false,
		1,
		bgfx::TextureFormat::Enum(format),
		BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
		bgfx::copy(pixels, size));
}


//
//	OtTexture::loadFromFileInMemory
//

void OtTexture::loadFromFileInMemory(void* data, uint32_t size) {
	// get the image
	OtImage image;
	image.loadFromFileInMemory(data, size);
	bimg::ImageContainer* container = image.getContainer();

	// update properties
	width = container->m_width;
	height = container->m_height;
	format = container->m_format;
	incrementVersion();

	// create texture
	texture = createRegularTexture(container);
}


//
//	OtTexture::update
//

void OtTexture::update(int x, int y, int w, int h, void* pixels) {
	// determine size of update
	auto size = bimg::imageGetSize(
		nullptr,
		static_cast<uint16_t>(w),
		static_cast<uint16_t>(h),
		1,
		false,
		false,
		1,
		bimg::TextureFormat::Enum(format));

	// update the texture
	bgfx::updateTexture2D(
		texture.getHandle(),
		0,
		0,
		static_cast<uint16_t>(x),
		static_cast<uint16_t>(y),
		static_cast<uint16_t>(w),
		static_cast<uint16_t>(h),
		bgfx::copy(pixels, size));

	incrementVersion();
}


//
//	OtTexture::getHandle
//

bgfx::TextureHandle OtTexture::getHandle() {
	// ensure we have a valid texture
	if (isValid()) {
		return texture.getHandle();

	} else {
		static bgfx::TextureHandle dummy = BGFX_INVALID_HANDLE;

		// create dummy texture (if required)
		if (!bgfx::isValid(dummy)) {
			OtImage image;
			bimg::ImageContainer* container = image.getContainer();
			const bgfx::Memory* mem = bgfx::copy(container->m_data, container->m_size);

			dummy = bgfx::createTexture2D(
				1, 1, false, 1,
				bgfx::TextureFormat::Enum(bimg::TextureFormat::R8),
				BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
				mem);

			// dummy texture will be destroyed when program exits
			OtFrameworkAtExit::add([]() {
				bgfx::destroy(dummy);
			});
		}

		// just return a dummy image to keep everybody happy
		return dummy;
	}
}
