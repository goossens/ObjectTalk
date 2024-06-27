//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <unordered_map>

#include "nanovg.h"

#include "OtFrameBuffer.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtTexture.h"
#include "OtUniformMat3.h"
#include "OtUniformVec4.h"


//
//	OtCanvas
//

class OtCanvas {
public:
	// constructors/destructor
	OtCanvas(int width, int height);
	~OtCanvas();

	// update canvas size
	void update(int width, int height);

	// release canvas data
	void clear();

	// start a new frame
	void startFrame();

	// render canvas to framebuffer
	void render(OtFrameBuffer& framebuffer);

private:
	// properties
	int width;
	int height;
	NVGcontext* context = nullptr;

	// canvas viewport
	float viewportWidth;
	float viewportHeight;
	float devicePixelRatio;

	// texture cache
	std::unordered_map<int, std::unique_ptr<OtTexture>> textures;

	// shader resources
	OtShaderProgram program{"OtCanvasVS", "OtCanvasFS"};
	OtUniformMat3 scissorMat{"u_scissorMat", 1};
	OtUniformMat3 paintMat{"u_paintMat", 1};
	OtUniformVec4 uniforms{"u_canvas", 7};
	OtUniformVec4 innerCol{"u_innerCol", 1};
	OtUniformVec4 outerCol{"u_outerCol", 1};
	OtUniformVec4 viewSize{"u_viewSize", 1};
	OtUniformVec4 scissorExtScale{"u_scissorExtScale", 1};
	OtUniformVec4 extentRadius{"u_extentRadius", 1};
	OtUniformVec4 params{"u_params", 1};
	OtSampler textureSampler{"s_texture"};

	// internal functions
	static int renderCreate(void* ptr);
	static int renderCreateTexture(void* ptr, int type, int w, int h, int imageFlags, const unsigned char* data);
	static int renderDeleteTexture(void* ptr, int image);
	static int renderUpdateTexture(void* ptr, int image, int x, int y, int w, int h, const unsigned char* data);
	static int renderGetTextureSize(void* ptr, int image, int* w, int* h);
	static void renderViewport(void* ptr, float width, float height, float devicePixelRatio);
	static void renderCancel(void* ptr);
	static void renderFlush(void* ptr);
	static void renderFill(void* ptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation, NVGscissor* scissor, float fringe, const float* bounds, const NVGpath* paths, int npaths);
	static void renderStroke(void* ptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation, NVGscissor* scissor, float fringe, float strokeWidth, int lineStyle, float lineLength, const NVGpath* paths, int npaths);
	static void renderTriangles(void* ptr, NVGpaint* paint, NVGcompositeOperationState compositeOperation, NVGscissor* scissor, const NVGvertex* verts, int nverts, float fringe, int text);
	static void renderDelete(void* ptr);
};
