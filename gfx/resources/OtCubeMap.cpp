//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bx/allocator.h"
#include "nlohmann/json.hpp"

#include "OtLog.h"

#include "OtAsset.h"
#include "OtCubeMap.h"
#include "OtFrameworkAtExit.h"
#include "OtImage.h"
#include "OtPass.h"
#include "OtPath.h"
#include "OtText.h"
#include "OtVertex.h"


//
//	OtCubeMap::create
//

void OtCubeMap::create(int s, bool m, int l, int f, uint64_t flags) {
	size = s;
	mip = m;
	layers = l;
	format = f;

	cubemap = bgfx::createTextureCube(
		static_cast<uint16_t>(size),
		mip,
		static_cast<uint16_t>(layers),
		bgfx::TextureFormat::Enum(format),
		flags);

	version++;
}


//
//	OtCubeMap::load
//

void OtCubeMap::load(const std::string& path, bool async) {
	auto ext = OtPath::getExtension(path);

	if (ext == ".json") {
		loadJSON(path, async);

	} else if (ext == ".hdr") {
		loadHdrImage(path, async);

	} else {
		loadCubemapImage(path, async);
	}
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

	auto posx = OtAssetDeserialize(&data, "posx", &basedir);
	auto negx = OtAssetDeserialize(&data, "negx", &basedir);
	auto posy = OtAssetDeserialize(&data, "posy", &basedir);
	auto negy = OtAssetDeserialize(&data, "negy", &basedir);
	auto posz = OtAssetDeserialize(&data, "posz", &basedir);
	auto negz = OtAssetDeserialize(&data, "negz", &basedir);

	if (posx.empty() || negx.empty() || posy.empty() || negy.empty() || posz.empty() || negz.empty()) {
		OtLogError("Incomplete CubeMap specification in [{}]", path);
	}

	// load first side
	tmpImages[0] = std::make_shared<OtImage>(posx, true, true);
	bimg::ImageContainer* container = tmpImages[0]->getContainer();
	size = static_cast<int>(container->m_width);
	bimg::TextureFormat::Enum imageFormat = container->m_format;
	mip = false;
	layers = 1;
	format = bgfx::TextureFormat::Enum(imageFormat);

	// load other sides
	tmpImages[1] = std::make_shared<OtImage>(negx, true, true);
	container = tmpImages[1]->getContainer();

	if (container->m_width != static_cast<uint32_t>(size) || container->m_format != imageFormat) {
		OtLogError("Cubemap image (negx] does not have same size or format as others");
	}

	tmpImages[2] = std::make_shared<OtImage>(posy, true, true);
	container = tmpImages[2]->getContainer();

	if (container->m_width != static_cast<uint32_t>(size) || container->m_format != imageFormat) {
		OtLogError("Cubemap image (posy] does not have same size or format as others");
	}

	tmpImages[3] = std::make_shared<OtImage>(negy, true, true);
	container = tmpImages[3]->getContainer();

	if (container->m_width != static_cast<uint32_t>(size) || container->m_format != imageFormat) {
		OtLogError("Cubemap image (negy] does not have same size or format as others");
	}

	tmpImages[4] = std::make_shared<OtImage>(posz, true, true);
	container = tmpImages[4]->getContainer();

	if (container->m_width != static_cast<uint32_t>(size) || container->m_format != imageFormat) {
		OtLogError("Cubemap image (posz] does not have same size or format as others");
	}

	tmpImages[5] = std::make_shared<OtImage>(negz, true, true);
	container = tmpImages[5]->getContainer();

	if (container->m_width != static_cast<uint32_t>(size) || container->m_format != imageFormat) {
		OtLogError("Cubemap image (negz] does not have same size or format as others");
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
//	OtCubeMap::loadCubemapImage
//

void OtCubeMap::loadCubemapImage(const std::string& path, bool async) {
	tmpImages[0] = std::make_shared<OtImage>(path);
	bimg::ImageContainer* container = tmpImages[0]->getContainer();

	// is this a real cubemap?
	if (!container->m_cubeMap) {
		OtLogError("Image is not a cubemap");
	}

	if (async) {
		// schedule a task to upload image to texture
		// we can't do that here as loading is done in a separate thread
		// the callback below will be called in the main thread
		asyncHandle = new uv_async_t;
		asyncHandle->data = this;

		auto status = uv_async_init(uv_default_loop(), asyncHandle, [](uv_async_t* handle){
			auto cubemap = (OtCubeMap*) handle->data;
			cubemap->createCubemapFromImage();

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
		createCubemapFromImage();
	}
}


//
//	OtCubeMap::loadHdrImage
//

void OtCubeMap::loadHdrImage(const std::string& path, bool async) {
	// load image
	tmpImages[0] = std::make_shared<OtImage>(path);

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
				cubemap->tmpTexture.clear();

				for (auto& framebuffer : cubemap->framebuffers) {
					framebuffer.clear();
				}
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
	// create a new cubemap
	cubemap = bgfx::createTextureCube(
		static_cast<uint16_t>(size),
		false,
		1,
		bgfx::TextureFormat::Enum(format),
		BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);

	// store sides
	bimg::ImageContainer* container = tmpImages[0]->getContainer();
	const bgfx::Memory* mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 0, 0, 0, 0, static_cast<uint16_t>(size), static_cast<uint16_t>(size), mem);

	container = tmpImages[1]->getContainer();
	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 1, 0, 0, 0, static_cast<uint16_t>(size), static_cast<uint16_t>(size), mem);

	container = tmpImages[2]->getContainer();
	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 2, 0, 0, 0, static_cast<uint16_t>(size), static_cast<uint16_t>(size), mem);

	container = tmpImages[3]->getContainer();
	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 3, 0, 0, 0, static_cast<uint16_t>(size), static_cast<uint16_t>(size), mem);

	container = tmpImages[4]->getContainer();
	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 4, 0, 0, 0, static_cast<uint16_t>(size), static_cast<uint16_t>(size), mem);

	container = tmpImages[5]->getContainer();
	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 5, 0, 0, 0, static_cast<uint16_t>(size), static_cast<uint16_t>(size), mem);
	version++;

	for (auto& image : tmpImages) {
		image = nullptr;
	}
}


//
//	OtCubeMap::createCubemapFromImage
//

void OtCubeMap::createCubemapFromImage() {
	bimg::ImageContainer* container = tmpImages[0]->getContainer();
	size = container->m_width;
	mip = container->m_numMips > 0;
	layers = container->m_numLayers;
	format = bgfx::TextureFormat::Enum(container->m_format);

	cubemap = bgfx::createTextureCube(
		static_cast<uint16_t>(container->m_width),
		false,
		1,
		bgfx::TextureFormat::Enum(container->m_format),
		BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
		bgfx::copy(container->m_data, container->m_size));

	tmpImages[0] = nullptr;
	version++;
}


//
//	OtCubeMap::createCubemapFromHDR
//

void OtCubeMap::createCubemapFromHDR() {
	// create a temporary texture
	bimg::ImageContainer* container = tmpImages[0]->getContainer();

	tmpTexture = bgfx::createTexture2D(
		uint16_t(container->m_width),
		uint16_t(container->m_height),
		false,
		container->m_numLayers,
		bgfx::TextureFormat::Enum(container->m_format),
		BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
		bgfx::copy(container->m_data, container->m_size));

	// create a unity cube
	static float cubeVertices[] = {
		// back face
		-1.0f, -1.0f, -1.0f,	// bottom-left
		 1.0f,  1.0f, -1.0f,	// top-right
		 1.0f, -1.0f, -1.0f,	// bottom-right
		 1.0f,  1.0f, -1.0f,	// top-right
		-1.0f, -1.0f, -1.0f,	// bottom-left
		-1.0f,  1.0f, -1.0f,	// top-left
		// front face
		-1.0f, -1.0f,  1.0f,	// bottom-left
		 1.0f, -1.0f,  1.0f,	// bottom-right
		 1.0f,  1.0f,  1.0f,	// top-right
		 1.0f,  1.0f,  1.0f,	// top-right
		-1.0f,  1.0f,  1.0f,	// top-left
		-1.0f, -1.0f,  1.0f,	// bottom-left
		// left face
		-1.0f,  1.0f,  1.0f,	// top-right
		-1.0f,  1.0f, -1.0f,	// top-left
		-1.0f, -1.0f, -1.0f,	// bottom-left
		-1.0f, -1.0f, -1.0f,	// bottom-left
		-1.0f, -1.0f,  1.0f,	// bottom-right
		-1.0f,  1.0f,  1.0f,	// top-right
		// right face
		 1.0f,  1.0f,  1.0f,	// top-left
		 1.0f, -1.0f, -1.0f,	// bottom-right
		 1.0f,  1.0f, -1.0f,	// top-right
		 1.0f, -1.0f, -1.0f,	// bottom-right
		 1.0f,  1.0f,  1.0f,	// top-left
		 1.0f, -1.0f,  1.0f,	// bottom-left
		// bottom face
		-1.0f, -1.0f, -1.0f,	// top-right
		 1.0f, -1.0f, -1.0f,	// top-left
		 1.0f, -1.0f,  1.0f,	// bottom-left
		 1.0f, -1.0f,  1.0f,	// bottom-left
		-1.0f, -1.0f,  1.0f,	// bottom-right
		-1.0f, -1.0f, -1.0f,	// top-right
		// top face
		-1.0f,  1.0f, -1.0f,	// top-left
		 1.0f,  1.0f , 1.0f,	// bottom-right
		 1.0f,  1.0f, -1.0f,	// top-right
		 1.0f,  1.0f,  1.0f,	// bottom-right
		-1.0f,  1.0f, -1.0f,	// top-left
		-1.0f,  1.0f,  1.0f,	// bottom-left
	};

	static constexpr size_t cubeVertexCount = sizeof(cubeVertices) / sizeof(*cubeVertices) / 3;

	if (bgfx::getAvailTransientVertexBuffer(cubeVertexCount, OtVertexPos::getLayout()) != cubeVertexCount) {
		OtLogFatal("Internal error: insufficient transient buffer space");
	}

	bgfx::TransientVertexBuffer tvb;
	bgfx::allocTransientVertexBuffer(&tvb, cubeVertexCount, OtVertexPos::getLayout());
	memcpy(tvb.data, cubeVertices, sizeof(cubeVertices));

	// define the projection matrix
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

	// define the view transformations
	glm::mat4 viewMatrices[] = {
		glm::lookAt(glm::vec3(0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f, 0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

	// create an empty cubemap
	size = 1024;
	mip = true;
	layers = 1;
	format = bgfx::TextureFormat::RGBA16F;
	version++;

	cubemap = bgfx::createTextureCube(
		static_cast<uint16_t>(size),
		mip,
		static_cast<uint16_t>(layers),
		bgfx::TextureFormat::RGBA16F,
		BGFX_TEXTURE_RT,
		nullptr);

	// render all 6 sides of the cube
	for (uint16_t side = 0; side < 6; side++) {
		// setup framebuffer
		bgfx::Attachment attachment;
		attachment.init(cubemap.getHandle(), bgfx::Access::Write, side);
		framebuffers[side] = bgfx::createFrameBuffer(1, &attachment);

		// setup a rendering pass
		OtPass pass;
		pass.setRectangle(0, 0, size, size);
		pass.setFrameBuffer(framebuffers[side].getHandle());
		pass.setTransform(viewMatrices[side], projectionMatrix);

		// setup sampler
		remapSampler.submit(0, tmpTexture.getHandle());

		// submit geometry
		bgfx::setVertexBuffer(0, &tvb);

		pass.setState(OtStateWriteRgb | OtStateWriteA | OtStateCullCcw);
		pass.runShaderProgram(reprojectShader);
	}
}


//
//	OtCubeMap::getHandle
//

bgfx::TextureHandle OtCubeMap::getHandle() {
	// ensure we have a valid cubemap
	if (isValid()) {
		return cubemap.getHandle();

	} else {
		static bgfx::TextureHandle dummy = BGFX_INVALID_HANDLE;

		// create dummy cubemap (if required)
		if (!bgfx::isValid(dummy)) {
			static bx::DefaultAllocator allocator;
			bimg::ImageContainer* image = bimg::imageAlloc(&allocator, bimg::TextureFormat::R8, 1, 1, 1, 1, true, false);
			const bgfx::Memory* mem = bgfx::copy(image->m_data, image->m_size);
			bimg::imageFree(image);

			dummy = bgfx::createTextureCube(
				1, false, 1,
				bgfx::TextureFormat::R8,
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
