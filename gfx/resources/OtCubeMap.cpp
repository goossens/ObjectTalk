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
//	Globals
//

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

void OtCubeMap::load(const std::string& path) {
	auto ext = OtPath::getExtension(path);

	if (ext == ".cubemap") {
		loadJSON(path);

	} else if (ext == ".hdr") {
		loadHdrImage(path);

	} else {
		loadCubemapImage(path);
	}
}


//
//	OtCubeMap::loadJSON
//

void OtCubeMap::loadJSON(const std::string& path) {
	// parse json
	std::string text;
	OtText::load(path, text);

	auto basedir = OtPath::getParent(path);
	auto data = nlohmann::json::parse(text);

	auto negx = OtAssetDeserialize(&data, "negx", &basedir);
	auto negy = OtAssetDeserialize(&data, "negy", &basedir);
	auto negz = OtAssetDeserialize(&data, "negz", &basedir);
	auto posx = OtAssetDeserialize(&data, "posx", &basedir);
	auto posy = OtAssetDeserialize(&data, "posy", &basedir);
	auto posz = OtAssetDeserialize(&data, "posz", &basedir);

	if (negx.empty() || negy.empty() || negz.empty() || posx.empty() || posy.empty() || posz.empty()) {
		OtLogError("Incomplete CubeMap specification in [{}]", path);
	}

	// load first side
	OtImage image;
	image.load(posx, true, true);
	bimg::ImageContainer* container = image.getContainer();
	size = static_cast<int>(container->m_width);
	bimg::TextureFormat::Enum imageFormat = container->m_format;
	mip = false;
	layers = 1;
	format = bgfx::TextureFormat::Enum(imageFormat);

	// create a new cubemap
	cubemap = bgfx::createTextureCube(
		static_cast<uint16_t>(size),
		false,
		1,
		bgfx::TextureFormat::Enum(imageFormat),
		BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);

	// store first side
	const bgfx::Memory* mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 0, 0, 0, 0, static_cast<uint16_t>(size), static_cast<uint16_t>(size), mem);

	// load and store other sides
	image.load(negx, true, true);
	container = image.getContainer();

	if (container->m_width != static_cast<uint32_t>(size) || container->m_format != imageFormat) {
		OtLogError("Cubemap image (negx] does not have same size or format as others");
	}

	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 1, 0, 0, 0, static_cast<uint16_t>(size), static_cast<uint16_t>(size), mem);

	image.load(posy, true, true);
	container = image.getContainer();

	if (container->m_width != static_cast<uint32_t>(size) || container->m_format != imageFormat) {
		OtLogError("Cubemap image (posy] does not have same size or format as others");
	}

	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 2, 0, 0, 0, static_cast<uint16_t>(size), static_cast<uint16_t>(size), mem);

	image.load(negy, true, true);
	container = image.getContainer();

	if (container->m_width != static_cast<uint32_t>(size) || container->m_format != imageFormat) {
		OtLogError("Cubemap image (negy] does not have same size or format as others");
	}

	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 3, 0, 0, 0, static_cast<uint16_t>(size), static_cast<uint16_t>(size), mem);

	image.load(posz, true, true);
	container = image.getContainer();

	if (container->m_width != static_cast<uint32_t>(size) || container->m_format != imageFormat) {
		OtLogError("Cubemap image (posz] does not have same size or format as others");
	}

	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 4, 0, 0, 0, static_cast<uint16_t>(size), static_cast<uint16_t>(size), mem);

	image.load(negz, true, true);
	container = image.getContainer();

	if (container->m_width != static_cast<uint32_t>(size) || container->m_format != imageFormat) {
		OtLogError("Cubemap image (negz] does not have same size or format as others");
	}

	mem = bgfx::copy(container->m_data, container->m_size);
	bgfx::updateTextureCube(cubemap.getHandle(), 0, 5, 0, 0, 0, static_cast<uint16_t>(size), static_cast<uint16_t>(size), mem);
	version++;
}


//
//	OtCubeMap::loadCubemapImage
//

void OtCubeMap::loadCubemapImage(const std::string& path) {
	// load image
	OtImage image;
	image.load(path, true, true);
	bimg::ImageContainer* container = image.getContainer();

	// is this a real cubemap?
	if (container->m_cubeMap) {
		cubemap = bgfx::createTextureCube(
			static_cast<uint16_t>(container->m_width),
			false,
			1,
			bgfx::TextureFormat::Enum(container->m_format),
			BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
			bgfx::copy(container->m_data, container->m_size));

	} else {
		OtLogError("Image is not a cubemap");
	}

	size = container->m_width;
	mip = container->m_numMips > 0;
	layers = container->m_numLayers;
	format = bgfx::TextureFormat::Enum(container->m_format);
	version++;
}


//
//	OtCubeMap::loadHdrImage
//

void OtCubeMap::loadHdrImage(const std::string& path) {
	// load image
	OtImage image;
	image.load(path);
	bimg::ImageContainer* container = image.getContainer();

	// create a temporary texture
	tmpTexture = bgfx::createTexture2D(
		uint16_t(container->m_width),
		uint16_t(container->m_height),
		false,
		container->m_numLayers,
		bgfx::TextureFormat::Enum(container->m_format),
		BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
		bgfx::copy(container->m_data, container->m_size));

	// schedule a task to convert an equirectangular HDR image to a cubemap
	// we can't do that here as loading is done asynchronously in a seperate thread
	// the callback below will be called in the main rendering thread
	hdrConversionHandle = new uv_async_t;
	hdrConversionHandle->data = this;

	auto status = uv_async_init(uv_default_loop(), hdrConversionHandle, [](uv_async_t* handle){
		// run the HDR to cubemap conversion
		auto cubemap = (OtCubeMap*) handle->data;
		cubemap->renderCubemap();

		// cleanup
		uv_close((uv_handle_t*) cubemap->hdrConversionHandle, [](uv_handle_t* handle) {
			auto cubemap = (OtCubeMap*) handle->data;
			delete (uv_fs_event_t*) handle;
			cubemap->hdrConversionHandle = nullptr;
			cubemap->tmpTexture.clear();

			for (auto& framebuffer : cubemap->framebuffers) {
				framebuffer.clear();
			}
		});
	});

	UV_CHECK_ERROR("uv_async_init", status);

	status = uv_async_send(hdrConversionHandle);
	UV_CHECK_ERROR("uv_async_send", status);
}


//
//	OtCubeMap::renderCubemap
//

void OtCubeMap::renderCubemap() {
	// create a unity cube
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
	size = 512;
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

		reprojectShader.setState(OtStateWriteRgb | OtStateWriteA | OtStateCullCcw);
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
