//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtLog.h"

#include "OtGpu.h"
#include "OtRenderShader.h"
#include "OtRenderPipeline.h"


//
//	OtRenderPipeline::setShaders
//

void OtRenderPipeline::setShaders(const uint32_t* vertexCode, size_t vertexSize, const uint32_t* fragmentCode, size_t fragmentSize) {
	vertexShaderCode = vertexCode;
	vertexShaderSize = vertexSize;
	fragmentShaderCode = fragmentCode;
	fragmentShaderSize = fragmentSize;
	pipeline = nullptr;
}


//
//	OtRenderPipeline::setVertexDescription
//

void OtRenderPipeline::setVertexDescription(OtVertexDescription* description) {
	vertexDescription = description;
	pipeline = nullptr;
}


//
//	OtRenderPipeline::setAnimationDescription
//

void OtRenderPipeline::setAnimationDescription(OtVertexDescription* description) {
	animationDescription = description;
	pipeline = nullptr;
}


//
//	OtRenderPipeline::setInstanceDescription
//

void OtRenderPipeline::setInstanceDescription(OtVertexDescription* description) {
	instanceDescription = description;
	pipeline = nullptr;
}


//
//	OtRenderPipeline::setRenderTargetType
//

void OtRenderPipeline::setRenderTargetType(RenderTargetType value) {
	renderTargetType = value;
	pipeline = nullptr;
}


//
//	OtRenderPipeline::setCulling
//

void OtRenderPipeline::setCulling(Culling value) {
	culling = value;
	pipeline = nullptr;
}


//
//	OtRenderPipeline::setFill
//

void OtRenderPipeline::setFill(bool mode) {
	fill = mode;
	pipeline = nullptr;
}


//
//	OtRenderPipeline::setColorMask
//

void OtRenderPipeline::setColorMask(ColorMask value) {
	useColorMask = true;
	colorMask = value;
	pipeline = nullptr;
}


//
//	OtRenderPipeline::setDepthTest
//

void OtRenderPipeline::setDepthTest(CompareOperation value) {
	useDepth = value != CompareOperation::none;
	depthTest = value;
	pipeline = nullptr;
}


//
//	OtRenderPipeline::setStencil
//

void OtRenderPipeline::setStencil(uint8_t compareMask, uint8_t writeMask, CompareOperation compare, StencilOperation pass, StencilOperation fail, StencilOperation depthFail) {
	setStencil(compareMask, writeMask, compare, pass, fail, depthFail, compare, pass, fail, depthFail);
}

void OtRenderPipeline::setStencil(uint8_t compareMask, uint8_t writeMask, CompareOperation frontCompare, StencilOperation frontPass, StencilOperation frontFail, StencilOperation frontDepthFail, CompareOperation backCompare, StencilOperation backPass, StencilOperation backFail, StencilOperation backDepthFail) {
	useStencilTest = (compareMask | writeMask) != 0;
	stencilCompareMask = compareMask;
	stencilWriteMask = writeMask;

	stencilFrontCompare = frontCompare;
	stencilFrontPassOperation = frontPass;
	stencilFrontFailOperation = frontFail;
	stencilFrontDepthFailOperation = frontDepthFail;

	stencilBackCompare = backCompare;
	stencilBackPassOperation = backPass;
	stencilBackFailOperation = backFail;
	stencilBackDepthFailOperation = backDepthFail;
	pipeline = nullptr;
}


//
//	OtRenderPipeline::setBlend
//

void OtRenderPipeline::setBlend(BlendOperation operation, BlendFactor src, BlendFactor dst) {
	setColorBlend(operation, src, dst);
	setAlphaBlend(operation, src, dst);
}


//
//	OtRenderPipeline::setColorBlend
//

void OtRenderPipeline::setColorBlend(BlendOperation operation, BlendFactor src, BlendFactor dst) {
	colorBlendOperation = operation;
	colorSrcFactor = src;
	colorDstFactor = dst;
	pipeline = nullptr;
}


//
//	OtRenderPipeline::setAlphaBlend
//

void OtRenderPipeline::setAlphaBlend(BlendOperation operation, BlendFactor src, BlendFactor dst) {
	alphaBlendOperation = operation;
	alphaSrcFactor = src;
	alphaDstFactor = dst;
	pipeline = nullptr;
}

void OtRenderPipeline::clear() {
	vertexShaderCode = nullptr;
	vertexShaderSize = 0;
	fragmentShaderCode = nullptr;
	fragmentShaderSize = 0;

	vertexDescription = nullptr;
	animationDescription = nullptr;
	instanceDescription = nullptr;
	renderTargetType = RenderTargetType::rgba8d32;
	culling = Culling::cw;
	fill = true;

	useColorMask = false;
	colorMask = ColorMask::rgba;

	useDepth = true;
	depthTest = CompareOperation::less;

	useStencilTest = false;
	stencilCompareMask = 0;
	stencilWriteMask = 0;

	stencilFrontCompare = CompareOperation::none;
	stencilFrontPassOperation = StencilOperation::none;
	stencilFrontFailOperation = StencilOperation::none;
	stencilFrontDepthFailOperation = StencilOperation::none;

	stencilBackCompare = CompareOperation::none;
	stencilBackPassOperation = StencilOperation::none;
	stencilBackFailOperation = StencilOperation::none;
	stencilBackDepthFailOperation = StencilOperation::none;

	colorBlendOperation = BlendOperation::none;
	alphaBlendOperation = BlendOperation::none;
	colorSrcFactor = BlendFactor::none;
	colorDstFactor = BlendFactor::none;
	alphaSrcFactor = BlendFactor::none;
	alphaDstFactor = BlendFactor::none;

	pipeline = nullptr;
}


//
//	OtRenderPipeline::assign
//

void OtRenderPipeline::assign(SDL_GPUGraphicsPipeline* newPipeline) {
	pipeline = std::shared_ptr<SDL_GPUGraphicsPipeline>(
		newPipeline,
		[](SDL_GPUGraphicsPipeline* oldPipeline) {
			auto& gpu = OtGpu::instance();
			SDL_ReleaseGPUGraphicsPipeline(gpu.device, oldPipeline);
			gpu.renderPipelines--;
		});

	OtGpu::instance().renderPipelines++;
}

//
//	OtRenderPipeline::getPipeline
//

SDL_GPUGraphicsPipeline* OtRenderPipeline::getPipeline() {
	// create pipeline (if required)
	if (!pipeline) {
		// ensure shaders are provided
		if (!vertexShaderCode || !vertexShaderSize || !vertexShaderCode || !vertexShaderSize) {
			OtLogFatal("Render pipeline is missing shaders");
		}

		// create shaders
		OtRenderShader vertexShader{vertexShaderCode, vertexShaderSize, OtRenderShader::Stage::vertex};
		OtRenderShader fragmentShader{fragmentShaderCode, fragmentShaderSize, OtRenderShader::Stage::fragment};

		// setup information
		SDL_GPUVertexInputState vertexInputState{};
		SDL_GPUVertexBufferDescription bufferDescriptions[3]{};
		size_t descriptionCount = 0;

		std::vector<SDL_GPUVertexAttribute> attributes;

		if (vertexDescription) {
			for (size_t i = 0; i < vertexDescription->members; i++) {
				auto& attribute = attributes.emplace_back(vertexDescription->attributes[i]);
				attribute.buffer_slot = static_cast<Uint32>(descriptionCount);
				attribute.location = static_cast<Uint32>(i);
			}

			bufferDescriptions[descriptionCount].pitch = static_cast<Uint32>(vertexDescription->size);
			bufferDescriptions[descriptionCount].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
			descriptionCount++;
		}

		if (animationDescription) {
			for (size_t i = 0; i < animationDescription->members; i++) {
				auto& attribute = attributes.emplace_back(animationDescription->attributes[i]);
				attribute.buffer_slot = static_cast<Uint32>(descriptionCount);
				attribute.location = static_cast<Uint32>(attributes.size() - 1);
			}

			bufferDescriptions[descriptionCount].slot = static_cast<Uint32>(descriptionCount);
			bufferDescriptions[descriptionCount].pitch = static_cast<Uint32>(animationDescription->size);
			bufferDescriptions[descriptionCount].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
			descriptionCount++;
		}

		if (instanceDescription) {
			for (size_t i = 0; i < instanceDescription->members; i++) {
				auto& attribute = attributes.emplace_back(instanceDescription->attributes[i]);
				attribute.buffer_slot = static_cast<Uint32>(descriptionCount);
				attribute.location = static_cast<Uint32>(attributes.size() - 1);
			}

			bufferDescriptions[descriptionCount].slot = static_cast<Uint32>(descriptionCount);
			bufferDescriptions[descriptionCount].pitch = static_cast<Uint32>(instanceDescription->size);
			bufferDescriptions[descriptionCount].input_rate = SDL_GPU_VERTEXINPUTRATE_INSTANCE;
			descriptionCount++;
		}

		if (descriptionCount) {
			vertexInputState.vertex_buffer_descriptions = bufferDescriptions;
			vertexInputState.num_vertex_buffers = static_cast<Uint32>(descriptionCount);
			vertexInputState.vertex_attributes = attributes.data();
			vertexInputState.num_vertex_attributes = static_cast<Uint32>(attributes.size());
		}

		std::vector<SDL_GPUColorTargetDescription> targetDescriptions;

		if (renderTargetType == RenderTargetType::gBuffer) {
			SDL_GPUColorTargetBlendState blendState{};
			blendState.enable_blend = false;
			blendState.enable_color_write_mask = false;
			targetDescriptions.emplace_back(SDL_GPUColorTargetDescription{SDL_GPU_TEXTUREFORMAT_R16G16B16A16_FLOAT, blendState});
			targetDescriptions.emplace_back(SDL_GPUColorTargetDescription{SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, blendState});
			targetDescriptions.emplace_back(SDL_GPUColorTargetDescription{SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,	blendState});
			targetDescriptions.emplace_back(SDL_GPUColorTargetDescription{SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, blendState});

		} else if (renderTargetType == RenderTargetType::cubemap) {
			SDL_GPUColorTargetBlendState blendState{};
			targetDescriptions.emplace_back(SDL_GPUColorTargetDescription{SDL_GPU_TEXTUREFORMAT_R16G16B16A16_FLOAT, blendState});
			targetDescriptions.emplace_back(SDL_GPUColorTargetDescription{SDL_GPU_TEXTUREFORMAT_R16G16B16A16_FLOAT, blendState});
			targetDescriptions.emplace_back(SDL_GPUColorTargetDescription{SDL_GPU_TEXTUREFORMAT_R16G16B16A16_FLOAT, blendState});
			targetDescriptions.emplace_back(SDL_GPUColorTargetDescription{SDL_GPU_TEXTUREFORMAT_R16G16B16A16_FLOAT, blendState});
			targetDescriptions.emplace_back(SDL_GPUColorTargetDescription{SDL_GPU_TEXTUREFORMAT_R16G16B16A16_FLOAT, blendState});
			targetDescriptions.emplace_back(SDL_GPUColorTargetDescription{SDL_GPU_TEXTUREFORMAT_R16G16B16A16_FLOAT, blendState});

		} else if (getTextureFormat() != SDL_GPU_TEXTUREFORMAT_INVALID) {
			SDL_GPUColorTargetBlendState blendState{};
			blendState.src_color_blendfactor = getBlendFactor(colorSrcFactor);
			blendState.dst_color_blendfactor = getBlendFactor(colorDstFactor);
			blendState.color_blend_op = getBlendOperation(colorBlendOperation);
			blendState.src_alpha_blendfactor = getBlendFactor(alphaSrcFactor);
			blendState.dst_alpha_blendfactor = getBlendFactor(alphaDstFactor);
			blendState.alpha_blend_op = getBlendOperation(alphaBlendOperation);
			blendState.color_write_mask = getColorMask();
			blendState.enable_blend = colorBlendOperation != BlendOperation::none || alphaBlendOperation != BlendOperation::none;
			blendState.enable_color_write_mask = useColorMask;
			targetDescriptions.emplace_back(SDL_GPUColorTargetDescription{getTextureFormat(), blendState});
		}

		SDL_GPURasterizerState rasterState{};
		rasterState.fill_mode = fill ? SDL_GPU_FILLMODE_FILL : SDL_GPU_FILLMODE_LINE;
		rasterState.cull_mode = getCullingMode();
		rasterState.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;

		SDL_GPUMultisampleState multisampleState{};
		multisampleState.sample_count = SDL_GPU_SAMPLECOUNT_1;

		SDL_GPUDepthStencilState depthStencilState{};
		depthStencilState.compare_op = getCompareOperation(depthTest);

		depthStencilState.back_stencil_state = SDL_GPUStencilOpState{
			getStencilOperation(stencilBackFailOperation),
			getStencilOperation(stencilBackPassOperation),
			getStencilOperation(stencilBackDepthFailOperation),
			getCompareOperation(stencilBackCompare)};

		depthStencilState.front_stencil_state = SDL_GPUStencilOpState{
			getStencilOperation(stencilFrontFailOperation),
			getStencilOperation(stencilFrontPassOperation),
			getStencilOperation(stencilFrontDepthFailOperation),
			getCompareOperation(stencilFrontCompare)};

		depthStencilState.compare_mask = static_cast<Uint8>(stencilCompareMask);
		depthStencilState.write_mask = static_cast<Uint8>(stencilWriteMask);
		depthStencilState.enable_depth_test = useDepth;
		depthStencilState.enable_depth_write = useDepth;
		depthStencilState.enable_stencil_test = useStencilTest;

		SDL_GPUGraphicsPipelineTargetInfo targetInfo{};
		targetInfo.color_target_descriptions = targetDescriptions.data();
		targetInfo.num_color_targets = static_cast<Uint32>(targetDescriptions.size());
		targetInfo.depth_stencil_format = getDepthStencilFormat();
		targetInfo.has_depth_stencil_target = hasDepthStencilFormat();

		SDL_GPUGraphicsPipelineCreateInfo createInfo{};
		createInfo.vertex_shader = vertexShader.getShader();
		createInfo.fragment_shader = fragmentShader.getShader();
		createInfo.vertex_input_state = vertexInputState;
		createInfo .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
		createInfo.rasterizer_state = rasterState;
		createInfo.multisample_state = multisampleState;
		createInfo.depth_stencil_state = depthStencilState;
		createInfo.target_info = targetInfo;

		// create graphics pipeline
		auto graphicsPipeline = SDL_CreateGPUGraphicsPipeline(OtGpu::instance().device, &createInfo);

		if (graphicsPipeline == nullptr) {
			OtLogFatal("Error in SDL_CreateGPUGraphicsPipeline: {}", SDL_GetError());
		}

		assign(graphicsPipeline);
	}

	// return the raw pipeline object
	return pipeline.get();
}


//
//	OtRenderPipeline::getTextureFormat
//

SDL_GPUTextureFormat OtRenderPipeline::getTextureFormat() {
	switch (renderTargetType) {
		case RenderTargetType::none: return SDL_GPU_TEXTUREFORMAT_INVALID;
		case RenderTargetType::r8: return SDL_GPU_TEXTUREFORMAT_R8_UNORM;
		case RenderTargetType::r8d32: return SDL_GPU_TEXTUREFORMAT_R8_UNORM;
		case RenderTargetType::rg16: return SDL_GPU_TEXTUREFORMAT_R16G16_UNORM;
		case RenderTargetType::rgba8: return SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
		case RenderTargetType::rgba16: return SDL_GPU_TEXTUREFORMAT_R16G16B16A16_FLOAT;
		case RenderTargetType::rgba32: return SDL_GPU_TEXTUREFORMAT_R32G32B32A32_FLOAT;
		case RenderTargetType::rgba8d32: return SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
		case RenderTargetType::rgba16d32: return SDL_GPU_TEXTUREFORMAT_R16G16B16A16_FLOAT;
		case RenderTargetType::rgba32d32: return SDL_GPU_TEXTUREFORMAT_R32G32B32A32_FLOAT;
		case RenderTargetType::rgba8d24s8: return SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
		case RenderTargetType::d32: return SDL_GPU_TEXTUREFORMAT_INVALID;
		case RenderTargetType::gBuffer: return SDL_GPU_TEXTUREFORMAT_INVALID;
		case RenderTargetType::cubemap: return SDL_GPU_TEXTUREFORMAT_INVALID;
	}

	return SDL_GPU_TEXTUREFORMAT_INVALID;
}


//
//	OtRenderPipeline::hasDepthStencilFormat
//

bool OtRenderPipeline::hasDepthStencilFormat() {
	switch (renderTargetType) {
		case RenderTargetType::none: return false;
		case RenderTargetType::r8: return false;
		case RenderTargetType::r8d32: return true;
		case RenderTargetType::rg16: return false;
		case RenderTargetType::rgba8: return false;
		case RenderTargetType::rgba16: return false;
		case RenderTargetType::rgba32: return false;
		case RenderTargetType::rgba8d32: return true;
		case RenderTargetType::rgba16d32: return true;
		case RenderTargetType::rgba32d32: return true;
		case RenderTargetType::rgba8d24s8: return true;
		case RenderTargetType::d32: return true;
		case RenderTargetType::gBuffer: return true;
		case RenderTargetType::cubemap: return false;
	}

	return false;
}


//
//	OtRenderPipeline::getDepthStencilFormat
//

SDL_GPUTextureFormat OtRenderPipeline::getDepthStencilFormat() {
	switch (renderTargetType) {
		case RenderTargetType::none: return SDL_GPU_TEXTUREFORMAT_INVALID;
		case RenderTargetType::r8: return SDL_GPU_TEXTUREFORMAT_INVALID;
		case RenderTargetType::r8d32: return SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
		case RenderTargetType::rg16: return SDL_GPU_TEXTUREFORMAT_INVALID;
		case RenderTargetType::rgba8: return SDL_GPU_TEXTUREFORMAT_INVALID;
		case RenderTargetType::rgba16: return SDL_GPU_TEXTUREFORMAT_INVALID;
		case RenderTargetType::rgba32: return SDL_GPU_TEXTUREFORMAT_INVALID;
		case RenderTargetType::rgba8d32: return SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
		case RenderTargetType::rgba16d32: return SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
		case RenderTargetType::rgba32d32: return SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
		case RenderTargetType::rgba8d24s8: return SDL_GPU_TEXTUREFORMAT_D32_FLOAT_S8_UINT;
		case RenderTargetType::d32: return SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
		case RenderTargetType::gBuffer: return SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
		case RenderTargetType::cubemap: return SDL_GPU_TEXTUREFORMAT_INVALID;
	}

	return SDL_GPU_TEXTUREFORMAT_INVALID;
}


//
//	OtRenderPipeline::getColorMask
//

SDL_GPUColorComponentFlags OtRenderPipeline::getColorMask() {
	SDL_GPUColorComponentFlags flags = 0;

	if (colorMask & ColorMask::r) {
		flags |= SDL_GPU_COLORCOMPONENT_R;
	}

	if (colorMask & ColorMask::g) {
		flags |= SDL_GPU_COLORCOMPONENT_G;
	}

	if (colorMask & ColorMask::b) {
		flags |= SDL_GPU_COLORCOMPONENT_B;
	}

	if (colorMask & ColorMask::a) {
		flags |= SDL_GPU_COLORCOMPONENT_A;
	}

	return flags;
}


//
//	OtRenderPipeline::getCompareOperation
//

SDL_GPUCompareOp OtRenderPipeline::getCompareOperation(CompareOperation operation) {
	switch (operation) {
		case CompareOperation::none: return SDL_GPU_COMPAREOP_INVALID;
		case CompareOperation::never: return SDL_GPU_COMPAREOP_NEVER;
		case CompareOperation::less: return SDL_GPU_COMPAREOP_LESS;
		case CompareOperation::equal: return SDL_GPU_COMPAREOP_EQUAL;
		case CompareOperation::lessEqual: return SDL_GPU_COMPAREOP_LESS_OR_EQUAL;
		case CompareOperation::greater: return SDL_GPU_COMPAREOP_GREATER;
		case CompareOperation::notEqual: return SDL_GPU_COMPAREOP_NOT_EQUAL;
		case CompareOperation::greaterEqual: return SDL_GPU_COMPAREOP_GREATER_OR_EQUAL;
		case CompareOperation::always: return SDL_GPU_COMPAREOP_ALWAYS;
	}

	return SDL_GPU_COMPAREOP_INVALID;
}


//
//	OtRenderPipeline::getCullingMode
//

SDL_GPUCullMode OtRenderPipeline::getCullingMode() {
	switch (culling) {
		case Culling::none: return SDL_GPU_CULLMODE_NONE;
		case Culling::cw: return SDL_GPU_CULLMODE_BACK;
		case Culling::ccw: return SDL_GPU_CULLMODE_FRONT;
	}

	return SDL_GPU_CULLMODE_NONE;
}


//
//	OtRenderPipeline::getBlendOperation
//

SDL_GPUBlendOp OtRenderPipeline::getBlendOperation(BlendOperation operation) {
	switch (operation) {
		case BlendOperation::none: return SDL_GPU_BLENDOP_INVALID;
		case BlendOperation::add: return SDL_GPU_BLENDOP_ADD;
		case BlendOperation::subtract: return SDL_GPU_BLENDOP_SUBTRACT;
		case BlendOperation::reverseSubtract: return SDL_GPU_BLENDOP_REVERSE_SUBTRACT;
		case BlendOperation::min: return SDL_GPU_BLENDOP_MIN;
		case BlendOperation::max: return SDL_GPU_BLENDOP_MAX;
	}

	return SDL_GPU_BLENDOP_INVALID;
}


//
//	OtRenderPipeline::getBlendFactor
//

SDL_GPUBlendFactor OtRenderPipeline::getBlendFactor(BlendFactor factor) {
	switch (factor) {
		case BlendFactor::none: return SDL_GPU_BLENDFACTOR_INVALID;
		case BlendFactor::zero: return SDL_GPU_BLENDFACTOR_ZERO;
		case BlendFactor::one: return SDL_GPU_BLENDFACTOR_ONE;
		case BlendFactor::srcColor: return SDL_GPU_BLENDFACTOR_SRC_COLOR;
		case BlendFactor::oneMinusSrcColor: return SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_COLOR;
		case BlendFactor::dstColor: return SDL_GPU_BLENDFACTOR_DST_COLOR;
		case BlendFactor::oneMinusDstColor: return SDL_GPU_BLENDFACTOR_ONE_MINUS_DST_COLOR;
		case BlendFactor::srcAlpha: return SDL_GPU_BLENDFACTOR_SRC_ALPHA;
		case BlendFactor::oneMinusSrcAlpha: return SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
		case BlendFactor::dstAlpha: return SDL_GPU_BLENDFACTOR_DST_ALPHA;
		case BlendFactor::oneMinusDstAlpha: return SDL_GPU_BLENDFACTOR_ONE_MINUS_DST_ALPHA;
		case BlendFactor::constantColor: return SDL_GPU_BLENDFACTOR_CONSTANT_COLOR;
		case BlendFactor::oneMinusConstantColor: return SDL_GPU_BLENDFACTOR_ONE_MINUS_CONSTANT_COLOR;
		case BlendFactor::alphaSaturate: return SDL_GPU_BLENDFACTOR_SRC_ALPHA_SATURATE;
	}

	return SDL_GPU_BLENDFACTOR_INVALID;
}


//
//	OtRenderPipeline::getStencilOperation
//

SDL_GPUStencilOp OtRenderPipeline::getStencilOperation(StencilOperation operation) {
	switch (operation) {
		case StencilOperation::none: return SDL_GPU_STENCILOP_INVALID;
		case StencilOperation::keep: return SDL_GPU_STENCILOP_KEEP;
		case StencilOperation::zero: return SDL_GPU_STENCILOP_ZERO;
		case StencilOperation::replace: return SDL_GPU_STENCILOP_REPLACE;
		case StencilOperation::incrementAndClamp: return SDL_GPU_STENCILOP_INCREMENT_AND_CLAMP;
		case StencilOperation::decrementAndClamp: return SDL_GPU_STENCILOP_DECREMENT_AND_CLAMP;
		case StencilOperation::invert: return SDL_GPU_STENCILOP_INVERT;
		case StencilOperation::incrementAndWrap: return SDL_GPU_STENCILOP_INCREMENT_AND_WRAP;
		case StencilOperation::decrementAndWrap: return SDL_GPU_STENCILOP_DECREMENT_AND_WRAP;
	}

	return SDL_GPU_STENCILOP_INVALID;
}
