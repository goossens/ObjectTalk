//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <vector>

#include "glm/glm.hpp"
#include "SDL3/SDL.h"

#include "OtCubeMap.h"
#include "OtFrameBuffer.h"
#include "OtGbuffer.h"
#include "OtGeometry.h"
#include "OtIndexBuffer.h"
#include "OtMesh.h"
#include "OtRenderPipeline.h"
#include "OtSampler.h"
#include "OtTexture.h"
#include "OtVertexBuffer.h"


//
//	OtRenderPass
//

class OtRenderPass {
public:
	// constructor/destructor
	OtRenderPass();
	~OtRenderPass();

	// set clear state (must be called before "start")
	// without calling this, the default is no clearing
	void setClearColor(bool flag, const glm::vec4& value=glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	void setClearDepth(bool flag, float value=1.0f);
	void setClearStencil(bool flag, uint8_t value=0);

	// start a render pass
	void start(OtTexture& texture);
	void start(OtFrameBuffer& framebuffer);
	void start(OtGbuffer& gbuffer);
	void start(OtCubeMap& cubemap, size_t mipLevel=0);

	// bind a render pipeline
	void bindPipeline(OtRenderPipeline& pipeline);

	// bind sampler
	void bindVertexSampler(size_t slot, OtSampler& sampler, SDL_GPUTexture* texture);
	void bindVertexSampler(size_t slot, OtSampler& sampler, OtTexture& texture);
	void bindVertexSampler(size_t slot, OtSampler& sampler, OtCubeMap& cubemap);

	void bindFragmentSampler(size_t slot, OtSampler& sampler, SDL_GPUTexture* texture);
	void bindFragmentSampler(size_t slot, OtSampler& sampler, OtTexture& texture);
	void bindFragmentSampler(size_t slot, OtSampler& sampler, OtCubeMap& cubemap);

	// set uniforms
	void bindVertexUniforms(size_t slot, const void* data, size_t size);
	void bindFragmentUniforms(size_t slot, const void* data, size_t size);

	// set the stencil reference
	void setStencilReference(uint8_t reference);

	// set instance data
	void setInstanceData(OtVertexBuffer& buffer);
	void setInstanceCount(size_t instances);

	// set animation data
	void setAnimationData(OtVertexBuffer& buffer);

	// execute a rendering command
	void render(size_t vertices, size_t instances=1);
	void render(OtVertexBuffer& buffer);

	void render(OtVertexBuffer& vertexBuffer, OtIndexBuffer& indexBuffer, size_t offset=0, size_t count=0);
	inline void render(OtMesh& mesh) { render(mesh.getVertexBuffer(), mesh.getIndexBuffer()); }
	inline void render(OtGeometry& geometry) { render(geometry.getMesh()); }

	// end a render pass
	void end();

private:
	// the GPU resource
	SDL_GPURenderPass* pass;

	// state
	bool open = false;
	SDL_GPUBuffer* animationBuffer = nullptr;
	SDL_GPUBuffer* instanceBuffer = nullptr;
	size_t instanceCount = 1;

	// clearing flags
	bool clearColorTexture = false;
	bool clearDepthTexture = false;
	bool clearStencilTexture = false;
	glm::vec4 clearColorValue{0.0f, 0.0f, 0.0f, 1.0f};
	float clearDepthValue = 1.0f;
	std::uint8_t clearStencilValue = 0;
};
