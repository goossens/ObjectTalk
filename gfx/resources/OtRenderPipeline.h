//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <memory>
#include <vector>

#include "SDL3/SDL.h"

#include "OtVertex.h"


//
//	OtRenderPipeline
//

class OtRenderPipeline {
public:
	// property types
	enum class RenderTargetType {
		none,
		r8,
		r8d32,
		rg16,
		rgba8,
		rgba16,
		rgba32,
		rgba8d32,
		rgba8d24s8,
		rgba16d32,
		rgba32d32,
		d32,
		gBuffer,
		cubemap
	};

	enum ColorMask {
		none = 0,
		r = 1 << 0,
		g = 1 << 1,
		b = 1 << 2,
		a = 1 << 3,
		rgb = (r | g | b),
		rgba = (r | g | b | a),
	};

	enum class CompareOperation {
		none,
		never,
		less,
		equal,
		lessEqual,
		greater,
		notEqual,
		greaterEqual,
		always
	};

	enum class Culling {
		none,
		cw,
		ccw
	};

	enum class BlendOperation {
		none,
		add,
		subtract,
		reverseSubtract,
		min,
		max
	};

	enum class BlendFactor {
		none,
		zero,
		one,
		srcColor,
		oneMinusSrcColor,
		dstColor,
		oneMinusDstColor,
		srcAlpha,
		oneMinusSrcAlpha,
		dstAlpha,
		oneMinusDstAlpha,
		constantColor,
		oneMinusConstantColor,
		alphaSaturate
	};

	enum class StencilOperation {
		none,
		keep,
		zero,
		replace,
		incrementAndClamp,
		decrementAndClamp,
		invert,
		incrementAndWrap,
		decrementAndWrap
	};

	// constructor
	OtRenderPipeline() = default;

	OtRenderPipeline(const uint32_t* vertexCode, size_t vertexSize, const uint32_t* fragmentCode, size_t fragmentSize) :
		vertexShaderCode(vertexCode),
		vertexShaderSize(vertexSize),
		fragmentShaderCode(fragmentCode),
		fragmentShaderSize(fragmentSize) {}

	// set pipeline properties
	void setShaders(const uint32_t* vertexCode, size_t vertexSize, const uint32_t* fragmentCode, size_t fragmentSize) ;
	void setVertexDescription(OtVertexDescription* description);
	void setAnimatedDescription(OtVertexDescription* description);
	void setInstanceDescription(OtVertexDescription* description);
	void setRenderTargetType(RenderTargetType value);
	void setCulling(Culling value);
	void setFill(bool mode);
	void setColorMask(ColorMask value);
	void setDepthTest(CompareOperation value);

	void setStencil(
		uint8_t compareMask, uint8_t writeMask,
		CompareOperation compare, StencilOperation pass, StencilOperation fail, StencilOperation depthFail);

	void setStencil(
		uint8_t compareMask, uint8_t writeMask,
		CompareOperation frontCompare, StencilOperation frontPass, StencilOperation frontFail, StencilOperation frontDepthFail,
		CompareOperation backCompare, StencilOperation backPass, StencilOperation backFail, StencilOperation backDepthFail);

	void setBlend(BlendOperation operation, BlendFactor src, BlendFactor dst);
	void setColorBlend(BlendOperation operation, BlendFactor src, BlendFactor dst);
	void setAlphaBlend(BlendOperation operation, BlendFactor src, BlendFactor dst);

	// clear the object
	void clear();

	// see if pipeline is valid
	inline bool isValid() { return pipeline != nullptr; }

private:
	// the GPU resource
	std::shared_ptr<SDL_GPUGraphicsPipeline> pipeline;

	// memory manage SDL resource
	void assign(SDL_GPUGraphicsPipeline* newPipeline);

	// pipeline properties
	const uint32_t* vertexShaderCode = nullptr;
	size_t vertexShaderSize = 0;
	const uint32_t* fragmentShaderCode = nullptr;
	size_t fragmentShaderSize = 0;

	OtVertexDescription* vertexDescription = nullptr;
	OtVertexDescription* animatedDescription = nullptr;
	OtVertexDescription* instanceDescription = nullptr;
	RenderTargetType renderTargetType = RenderTargetType::rgba8d32;
	Culling culling = Culling::none;
	bool fill = true;

	bool useColorMask = false;
	ColorMask colorMask = ColorMask::rgba;

	bool useDepth = false;
	CompareOperation depthTest = CompareOperation::none;

	bool useStencilTest = false;
	uint8_t stencilCompareMask = 0xff;
	uint8_t stencilWriteMask = 0xff;

	CompareOperation stencilFrontCompare = CompareOperation::none;
	StencilOperation stencilFrontPassOperation = StencilOperation::none;
	StencilOperation stencilFrontFailOperation = StencilOperation::none;
	StencilOperation stencilFrontDepthFailOperation = StencilOperation::none;

	CompareOperation stencilBackCompare = CompareOperation::none;
	StencilOperation stencilBackPassOperation = StencilOperation::none;
	StencilOperation stencilBackFailOperation = StencilOperation::none;
	StencilOperation stencilBackDepthFailOperation = StencilOperation::none;

	BlendOperation colorBlendOperation = BlendOperation::none;
	BlendOperation alphaBlendOperation = BlendOperation::none;
	BlendFactor colorSrcFactor = BlendFactor::none;
	BlendFactor colorDstFactor = BlendFactor::none;
	BlendFactor alphaSrcFactor = BlendFactor::none;
	BlendFactor alphaDstFactor = BlendFactor::none;

	// get the raw pipeline object
	friend class OtRenderPass;
	SDL_GPUGraphicsPipeline* getPipeline();

	// translate properties
	SDL_GPUTextureFormat getTextureFormat();
	bool hasDepthStencilFormat();
	SDL_GPUTextureFormat getDepthStencilFormat();
	SDL_GPUColorComponentFlags getColorMask();
	SDL_GPUCompareOp getCompareOperation(CompareOperation operation);
	SDL_GPUCullMode getCullingMode();
	SDL_GPUBlendOp getBlendOperation(BlendOperation operation);
	SDL_GPUBlendFactor getBlendFactor(BlendFactor factor);
	SDL_GPUStencilOp getStencilOperation(StencilOperation operation);
};
