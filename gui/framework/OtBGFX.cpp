//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>

#include "bx/timer.h"
#include "bx/allocator.h"
#include "bx/file.h"
#include "bimg/decode.h"
#include "debugdraw.h"
#include "imgui.h"

#include "OtException.h"

#include "OtFramework.h"


//
//	OtFrameworkClass::initBGFX
//

void OtFrameworkClass::initBGFX() {
	// initialize bgfx
	bgfx::Init init;

#if __APPLE__
	init.type = bgfx::RendererType::Metal;

#elif defined(_WIN32)
	init.type = bgfx::RendererType::Direct3D12;

#else
	init.type = bgfx::RendererType::OpenGLES;
#endif

	init.platformData.nwh = nativeDisplayHandle;
	init.platformData.ndt = nativeDisplayType;
	init.resolution.width  = width;
	init.resolution.height = height;
	init.resolution.reset  = BGFX_RESET_VSYNC;
	init.debug = false;

	bgfx::init(init);

	// ensure this platform has the right capabilities
	const bgfx::Caps* caps = bgfx::getCaps();

	if (!(caps->supported & BGFX_CAPS_RENDERER_MULTITHREADED)) {
		OtExcept("Your system/graphics card does not support multithreading");
	}

	if (!(caps->supported & BGFX_CAPS_INDEX32)) {
		OtExcept("Your system/graphics card does not support 32 bit vertex indexing");
	}

	if (!(caps->supported & BGFX_CAPS_INSTANCING)) {
		OtExcept("Your system/graphics card does not support instancing");
	}

	if (!(caps->supported & BGFX_CAPS_TEXTURE_COMPARE_LEQUAL)) {
		OtExcept("Your system/graphics card does not support texture '<='");
	}

	// initialize debug draw
	ddInit();

	// initialize time management
	startTime = lastTime = bx::getHPCounter();

	// create dummy image and texture
	static bx::DefaultAllocator allocator;
	dummyImage = bimg::imageAlloc(&allocator, bimg::TextureFormat::R8, 1, 1, 0, 1, false, false);
	const bgfx::Memory* mem = bgfx::makeRef(dummyImage->m_data, dummyImage->m_size);

	dummyTexture = bgfx::createTexture2D(
		1, 1, 0, 1,
		bgfx::TextureFormat::Enum(bimg::TextureFormat::R8),
		BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
		mem);
}


//
//	OtFrameworkClass::frameBGFX
//

void OtFrameworkClass::frameBGFX() {
	// get time since epoch
	loopTime = bx::getHPCounter();

	// calculate loop speed
	if (loopTime >= lastTime) {
		loopDuration = (double) (loopTime - lastTime) / (double) bx::getHPFrequency() * 1000.0;

	} else {
		loopDuration = 1.0 / 60.0 * 1000.0;
	}

	lastTime = loopTime;

	// reset graphic settings and back-buffer size (if required)
	static int prevWidth = 0;
	static int prevHeight = 0;
	static int prevAA = 0;

	if (width != prevWidth || height != prevHeight || antiAliasing != prevAA) {
		int AA;

		switch (antiAliasing) {
			case 1: AA = BGFX_RESET_MSAA_X2; break;
			case 2: AA = BGFX_RESET_MSAA_X4; break;
			case 3: AA = BGFX_RESET_MSAA_X8; break;
			case 4: AA = BGFX_RESET_MSAA_X16; break;
			default: AA = 0; break;
		}

		bgfx::reset(width, height, BGFX_RESET_VSYNC | AA);
		prevWidth = width;
		prevHeight = height;
		prevAA = antiAliasing;
	}

	// start BGFX frame by clearing the screen
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000ff, 1.0f, 0);
	bgfx::setViewRect(0, 0, 0, width, height);
	bgfx::touch(0);
}


//
//	OtFrameworkClass::renderProfiler
//

void OtFrameworkClass::renderProfiler() {
	const bgfx::Stats* stats = bgfx::getStats();
	const double toMs = 1000.0 / (double) bx::getHPFrequency();

	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::Begin("Profiler", nullptr, 0);
	ImGui::Text("Framerate:"); ImGui::SameLine(150); ImGui::Text("%.1f", getFrameRate());
	ImGui::Text("CPU [ms per frame]:"); ImGui::SameLine(150); ImGui::Text("%0.1f", (double) cpuDuration * toMs);
	ImGui::Text("GPU [ms per frame]:"); ImGui::SameLine(150); ImGui::Text("%0.1f", (double) gpuDuration * toMs);
	ImGui::Text("Wait render [ms]:"); ImGui::SameLine(150); ImGui::Text("%0.1f", (double) stats->waitRender * toMs);
	ImGui::Text("Wait submit [ms]:"); ImGui::SameLine(150); ImGui::Text("%0.1f", (double) stats->waitSubmit * toMs);
	ImGui::Text("Backbuffer width:"); ImGui::SameLine(150); ImGui::Text("%d", stats->width);
	ImGui::Text("Backbuffer height:"); ImGui::SameLine(150); ImGui::Text("%d", stats->height);
	ImGui::Text("Anti-aliasing:"); ImGui::SameLine(150); ImGui::Text("%d", antiAliasing);
	ImGui::Text("Views:"); ImGui::SameLine(150); ImGui::Text("%d", nextViewID + 2);
	ImGui::Text("Draw calls:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numDraw);
	ImGui::Text("Programs:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numPrograms);
	ImGui::Text("Shaders:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numShaders);
	ImGui::Text("Textures:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numTextures);
	ImGui::Text("Uniforms:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numUniforms);
	ImGui::Text("Vertex buffers:"); ImGui::SameLine(150); ImGui::Text("%d", stats->numVertexBuffers);
	ImGui::End();
}


//
//	OtFrameworkClass::getTime
//

float OtFrameworkClass::getTime() {
	return (double) (loopTime - startTime) / (double) bx::getHPFrequency();
}




//
//	OtFrameworkClass::getImage
//

bimg::ImageContainer* OtFrameworkClass::getImage(const std::string& file, bool powerof2, bool square) {
	// load image if required
	if (!imageRegistry.has(file)) {
		// load named image
		static bx::DefaultAllocator allocator;
		static bx::FileReader reader;

		if (!bx::open(&reader, file.c_str())) {
			OtExcept("Can't open image [%s]", file.c_str());
		}

		uint32_t size = (uint32_t) bx::getSize(&reader);
		void* data = BX_ALLOC(&allocator, size);
		bx::read(&reader, data, size, bx::ErrorAssert{});
		bx::close(&reader);

		bimg::ImageContainer* image = bimg::imageParse(&allocator, data, size);
		BX_FREE(&allocator, data);

		if (!image) {
			OtExcept("Can't process image in [%s]", file.c_str());
		}

		// validate sides are power of 2 (if required)
		if (powerof2 && !(bx::isPowerOf2(image->m_width))) {
			bimg::imageFree(image);
			OtExcept("Image width %d is not a power of 2", image->m_width);
		}

		if (powerof2 && !(bx::isPowerOf2(image->m_height))) {
			bimg::imageFree(image);
			OtExcept("Image height %d is not a power of 2", image->m_height);
		}

		// validate squareness (if required)
		if (square && image->m_width != image->m_height) {
			bimg::imageFree(image);
			OtExcept("Image must be square not %d by %d", image->m_width, image->m_height);
		}

		// add image to registry
		imageRegistry.set(file, image);
	}

	// return image
	return imageRegistry.get(file);
}


//
//	OtFrameworkClass::getTexture
//

bgfx::TextureHandle OtFrameworkClass::getTexture(const std::string& file, bimg::ImageContainer** image) {
	bgfx::TextureHandle texture;

	// create texture if required
	if (!textureRegistry.has(file)) {
		// get the image
		bimg::ImageContainer* image = getImage(file);
		const bgfx::Memory* mem = bgfx::makeRef(image->m_data, image->m_size);

		// create texture
		texture = bgfx::createTexture2D(
			uint16_t(image->m_width),
			uint16_t(image->m_height),
			1 < image->m_numMips,
			image->m_numLayers,
			bgfx::TextureFormat::Enum(image->m_format),
			BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
			mem);

		// add texture to registry
		textureRegistry.set(file, texture);
		textureImageMap[texture.idx] = image;

	} else {
		texture = textureRegistry.get(file);
	}

	// return (if required)
	if (image) {
		*image = textureImageMap[texture.idx];
	}

	// return texture
	return texture;
}


//
//	OtFrameworkClass::renderBGFX
//

void OtFrameworkClass::renderBGFX() {
	// get CPU and GPU statistics
	cpuDuration = bx::getHPCounter() - loopTime;

	const bgfx::Stats* stats = bgfx::getStats();
	gpuDuration = stats->gpuTimeEnd - stats->gpuTimeBegin;

	// render BGFX frame
	bgfx::frame();
}


//
//	OtFrameworkClass::endBGFX
//

void OtFrameworkClass::endBGFX() {
	bimg::imageFree(dummyImage);
	bgfx::destroy(dummyTexture);

	textureRegistry.iterateValues([] (bgfx::TextureHandle& texture){
		bgfx::destroy(texture);
	});

	imageRegistry.iterateValues([] (bimg::ImageContainer*& image){
		bimg::imageFree(image);
	});

	ddShutdown();
	bgfx::shutdown();
}
