//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFrameBuffer.h"
#include "OtRenderLight.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtUniformVec4.h"

#include "OtOcclusionPass.h"


//
//	OtPostProcessingPass
//

class OtPostProcessingPass {
public:
	// constructor
	OtPostProcessingPass(OtFrameBuffer& fb) : framebuffer(fb) {}

	// render the pass
	void render(OtSceneRendererContext& ctx);

private:
	// give the debugger access to the inner circle
	friend class OtSceneRendererDebug;

	// properties
	OtFrameBuffer& framebuffer;
	OtFrameBuffer postProcessBuffer1{OtTexture::rgbaFloat16Texture, OtTexture::noTexture};
	OtFrameBuffer postProcessBuffer2{OtTexture::rgbaFloat16Texture, OtTexture::noTexture};
	OtFrameBuffer occlusionBuffer{OtTexture::r8Texture, OtTexture::noTexture};

	OtRenderLight renderLight;
	OtOcclusionPass occlusionPass{occlusionBuffer};

	static constexpr int bloomDepth = 5;
	OtFrameBuffer bloomBuffer[bloomDepth];

	OtUniformVec4 fxaaUniforms{"u_fxaa", 1};
	OtUniformVec4 fogUniforms{"u_fog", 2};
	OtUniformVec4 bloomUniforms{"u_bloom", 1};
	OtUniformVec4 godrayUniforms{"u_godrays", 3};
	OtUniformVec4 postProcessUniforms{"u_postProcess", 1};

	OtUniformMat4 invProjUniform{"u_invProjUniform", 1};

	OtSampler postProcessSampler{"s_postProcessTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler depthSampler{"s_depthTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler bloomSampler{"s_bloomTexture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtSampler occlusionSampler{"s_occlusionTexture", OtSampler::pointSampling | OtSampler::clampSampling};

	OtShaderProgram fxaaProgram{"OtFilterVS", "OtFxaaFS"};
	OtShaderProgram fogProgram{"OtFilterVS", "OtFogFS"};
	OtShaderProgram bloomDownSampleProgram{"OtFilterVS", "OtBloomDownSampleFS"};
	OtShaderProgram bloomUpSampleProgram{"OtFilterVS", "OtBloomUpSampleFS"};
	OtShaderProgram bloomApplyProgram{"OtFilterVS", "OtBloomApplyFS"};
	OtShaderProgram godrayProgram{"OtGodraysVS", "OtGodraysFS"};
	OtShaderProgram postProcessProgram{"OtFilterVS", "OtPostProcessFS"};

	// rendering functions
	void renderFxaa(OtSceneRendererContext& ctx, OtFrameBuffer* input, OtFrameBuffer* output);
	void renderFog(OtSceneRendererContext& ctx, OtFrameBuffer* input, OtFrameBuffer* output, float fogDensity, glm::vec3& fogColor);
	void renderBloom(OtSceneRendererContext& ctx, OtFrameBuffer* input, OtFrameBuffer* output, float bloomIntensity);
	void renderGodrays(OtSceneRendererContext& ctx, OtFrameBuffer* input, OtFrameBuffer* output, glm::vec2& uv);
};
