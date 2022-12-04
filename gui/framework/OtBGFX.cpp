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
#include "mipmap.h"

#include "OtException.h"
#include "OtFormat.h"

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
		1, 1, false, 1,
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
//	OtFrameworkClass::getUniform
//

bgfx::UniformHandle OtFrameworkClass::getUniform(const char* name, bgfx::UniformType::Enum type, uint16_t size) {
	// create program (if required)
	if (!uniformRegistry.has(name)) {
		bgfx::UniformHandle uniform = bgfx::createUniform(name, type, size);
		uniformRegistry.set(name, uniform);
		return uniform;

	} else {
		return uniformRegistry.get(name);
	}
}


//
//	OtFrameworkClass::getProgram
//

bgfx::ProgramHandle OtFrameworkClass::getProgram(const bgfx::EmbeddedShader* shaders, const char* vertex, const char* fragment) {
	std::string index = OtFormat("%s:%s", vertex, fragment);

	// create program (if required)
	if (!programRegistry.has(index)) {
		bgfx::RendererType::Enum type = bgfx::getRendererType();

		bgfx::ProgramHandle program =  bgfx::createProgram(
			bgfx::createEmbeddedShader(shaders, type, vertex),
			bgfx::createEmbeddedShader(shaders, type, fragment),
			true);

		programRegistry.set(index, program);
		return program;

	} else {
		return programRegistry.get(index);
	}
}


//
//	createRegularTexture
//

static bgfx::TextureHandle createRegularTexture(bimg::ImageContainer* image) {
	return bgfx::createTexture2D(
		uint16_t(image->m_width),
		uint16_t(image->m_height),
		false,
		image->m_numLayers,
		bgfx::TextureFormat::Enum(image->m_format),
		BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE,
		bgfx::makeRef(image->m_data, image->m_size));
}


//
//	createMipmapTexture
//

static bgfx::TextureHandle createMipmapTexture(bimg::ImageContainer* image) {
	// create a new empty texture
	bgfx::TextureHandle texture = bgfx::createTexture2D(
		uint16_t(image->m_width),
		uint16_t(image->m_height),
		true,
		image->m_numLayers,
		bgfx::TextureFormat::Enum(image->m_format),
		BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);

	const bimg::ImageBlockInfo& blockInfo = getBlockInfo(image->m_format);
	const uint32_t blockWidth  = blockInfo.blockWidth;
	const uint32_t blockHeight = blockInfo.blockHeight;

	uint32_t width = image->m_width;
	uint32_t height = image->m_height;

	// process all mip levels
	for (auto lod = 0; lod < image->m_numMips; lod++) {
		width = std::max(blockWidth,  width);
		height = std::max(blockHeight, height);

		bimg::ImageMip mip;

		if (bimg::imageGetRawData(*image, 0, lod, image->m_data, image->m_size, mip)) {
			bgfx::updateTexture2D(
				texture,
				0, lod,
				0, 0,
				uint16_t(width), uint16_t(height),
				bgfx::copy(mip.m_data, mip.m_size));
		}

		width  >>= 1;
		height >>= 1;
	}

	return texture;
}


//
//	generateMipmapTexture
//

static bgfx::TextureHandle generateMipmapTexture(bimg::ImageContainer* image) {
	// sanity check
	auto bpp = bimg::getBitsPerPixel(image->m_format);

	if (bpp != 24 && bpp != 32) {
		OtExcept("Can't generate MipMap for image with %d bits per pixel", bpp);
	}

	// create a new empty texture
	bgfx::TextureHandle texture = bgfx::createTexture2D(
		uint16_t(image->m_width),
		uint16_t(image->m_height),
		true,
		image->m_numLayers,
		bgfx::TextureFormat::Enum(image->m_format),
		BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);

	// add first level
	bgfx::updateTexture2D(
		texture,
		0, 0, 0, 0,
		uint16_t(image->m_width), uint16_t(image->m_height),
		bgfx::makeRef(image->m_data, image->m_size));

	// create mipmaps
	AdmBitmap bitmap;
	bitmap.width = image->m_width;
	bitmap.height = image->m_height;
	bitmap.bytes_per_pixel = bpp / 8;
	bitmap.pixels = image->m_data;

	int levels;
	AdmBitmap* mipmaps = adm_generate_mipmaps_no_base(&levels, &bitmap);

	for (auto level = 0; level < levels; level++) {
		auto width = mipmaps[level].width;
		auto height = mipmaps[level].height;
		auto bpp = mipmaps[level].bytes_per_pixel;
		auto pixels = mipmaps[level].pixels;

		bgfx::updateTexture2D(
			texture,
			0, level + 1,
			0, 0,
			uint16_t(width), uint16_t(height),
			bgfx::copy(pixels, width * height * bpp));
	}

	// cleanup
	adm_free_mipmaps(mipmaps, levels);

	return texture;
}


//
//	OtFrameworkClass::getTexture
//

bgfx::TextureHandle OtFrameworkClass::getTexture(const std::string& file, bool mipmap, bimg::ImageContainer** image) {
	bgfx::TextureHandle texture;

	// create texture if required
	if (!textureRegistry.has(file)) {
		// get the image
		bimg::ImageContainer* image = getImage(file);

		// create texture
		if (image->m_numMips > 1) {
			texture = createMipmapTexture(image);

		} else if (mipmap) {
			texture = generateMipmapTexture(image);

		} else {
			texture = createRegularTexture(image);
		}

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
//	OtFrameworkClass::getImage
//

bimg::ImageContainer* OtFrameworkClass::getImage(const std::string& file, bool powerof2, bool square) {
	// load image (if required)
	if (!imageRegistry.has(file)) {
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
		return image;

	} else {
		// return image
		return imageRegistry.get(file);
	}
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
	// cleanup
	bgfx::destroy(dummyTexture);
	bimg::imageFree(dummyImage);

	uniformRegistry.iterateValues([] (bgfx::UniformHandle& uniform){
		bgfx::destroy(uniform);
	});

	programRegistry.iterateValues([] (bgfx::ProgramHandle& program){
		bgfx::destroy(program);
	});

	textureRegistry.iterateValues([] (bgfx::TextureHandle& texture){
		bgfx::destroy(texture);
	});

	imageRegistry.iterateValues([] (bimg::ImageContainer*& image){
		bimg::imageFree(image);
	});

	// shutdown BGFX
	ddShutdown();
	bgfx::shutdown();
}
