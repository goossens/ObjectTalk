//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtLog.h"

#include "OtCanvas.h"
#include "OtPass.h"


//
//	OtCanvas::OtCanvas
//

OtCanvas::OtCanvas(int width, int height) {
	update(width, height);
}


//
//	OtCanvas::~OtCanvas
//

OtCanvas::~OtCanvas() {
	clear();
}


//
//	OtCanvas::update
//

void OtCanvas::update(int width, int height) {
	// cleanup first
	clear();

	// create a new rendering context
	struct NVGparams params;
	params.userPtr = this;
	params.edgeAntiAlias = 1;
	params.renderCreate = renderCreate;
	params.renderCreateTexture = renderCreateTexture;
	params.renderDeleteTexture = renderDeleteTexture;
	params.renderUpdateTexture = renderUpdateTexture;
	params.renderGetTextureSize = renderGetTextureSize;
	params.renderViewport = renderViewport;
	params.renderFlush = renderFlush;
	params.renderFill = renderFill;
	params.renderStroke = renderStroke;
	params.renderTriangles = renderTriangles;
	params.renderDelete = renderDelete;
	context = nvgCreateInternal(&params);

	if (!context) {
		OtLogFatal("Internal error: can't create a new canvas context");
	}
}


//
//	OtCanvas::clear
//

void OtCanvas::clear() {
	if (context) {
		nvgDeleteInternal(context);
		context = nullptr;
	}
}


//
//	OtCanvas::render
//

void OtCanvas::render(OtFrameBuffer &framebuffer) {
	// setup rendering pass
	OtPass pass;
	pass.touch();
	auto view = pass.getViewId();
	bgfx::setViewMode(view, bgfx::ViewMode::Sequential);
}


//
//	OtCanvas::renderCreate
//

int OtCanvas::renderCreate(void* ptr) {
	OtCanvas* canvas = (OtCanvas*) ptr;
	return 1;
}


//
//	OtCanvas::renderCreate
//

int OtCanvas::renderCreateTexture(void* ptr, int type, int w, int h, int imageFlags, const unsigned char* data) {
	OtCanvas* canvas = (OtCanvas*) ptr;

	// create the texture
	auto texture = std::make_unique<OtTexture>();
	auto format = (type == NVG_TEXTURE_RGBA) ? OtTexture::rgba8Texture : OtTexture::r8Texture;

	if (data) {
		texture->loadFromMemory(w, h, format, (void*) data);

	} else {
		texture->create(w, h, format);
	}

	// add it to the cache
	auto index = texture->getIndex();
	canvas->textures[index] = std::move(texture);
	return index;
}


//
//	OtCanvas::renderDeleteTexture
//

int OtCanvas::renderDeleteTexture(void* ptr, int image) {
	OtCanvas* canvas = (OtCanvas*) ptr;

	// find and delete texture
	auto entry = canvas->textures.find(image);

	if (entry != canvas->textures.end()) {
		canvas->textures.erase(entry);
	}

	return 1;
}


//
//	OtCanvas::renderUpdateTexture
//

int OtCanvas::renderUpdateTexture(void* ptr, int image, int x, int y, int w, int h, const unsigned char* data) {
	OtCanvas* canvas = (OtCanvas*) ptr;

	// find texture
	auto entry = canvas->textures.find(image);

	if (entry == canvas->textures.end()) {
		OtLogFatal("Can't find texture in canvas with id {}", image);
	}

	// update the texture
	entry->second->update(x, y, w, h, (void*) data);
	return 1;
}


//
//	OtCanvas::renderGetTextureSize
//

int OtCanvas::renderGetTextureSize(void* ptr, int image, int* w, int* h) {
	OtCanvas* canvas = (OtCanvas*) ptr;

	// find texture
	auto entry = canvas->textures.find(image);

	if (entry == canvas->textures.end()) {
		OtLogFatal("Can't find texture in canvas with id {}", image);
	}

	// get the size
	*w = entry->second->getWidth();
	*h = entry->second->getHeight();
	return 1;
}


//
//	OtCanvas::renderViewport
//

void OtCanvas::renderViewport(void* ptr, float width, float height, float devicePixelRatio) {
	OtCanvas* canvas = (OtCanvas*) ptr;
	canvas->viewportWidth = width;
	canvas->viewportHeight = height;
	canvas->devicePixelRatio = devicePixelRatio;
}


//
//	OtCanvas::renderCancel
//

void OtCanvas::renderCancel(void* ptr) {
	OtCanvas* canvas = (OtCanvas*) ptr;
}


//
//	OtCanvas::renderFlush
//

void OtCanvas::renderFlush(void* ptr) {
	OtCanvas* canvas = (OtCanvas*) ptr;
}


//
//	OtCanvas::renderFill
//

void OtCanvas::renderFill(void* ptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation, NVGscissor* scissor, float fringe, const float* bounds, const NVGpath* paths, int npaths) {
	OtCanvas* canvas = (OtCanvas*) ptr;
}


//
//	OtCanvas::renderStroke
//

void OtCanvas::renderStroke(void* ptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation, NVGscissor* scissor, float fringe, float strokeWidth, int lineStyle, float lineLength, const NVGpath* paths, int npaths) {
	OtCanvas* canvas = (OtCanvas*) ptr;
}


//
//	OtCanvas::renderTriangles
//

void OtCanvas::renderTriangles(void* ptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation, NVGscissor* scissor, const NVGvertex* verts, int nverts, float fringe, int text) {
	OtCanvas* canvas = (OtCanvas*) ptr;
}


//
//	OtCanvas::renderDelete
//

void OtCanvas::renderDelete(void* ptr) {
	OtCanvas* canvas = (OtCanvas*) ptr;
}