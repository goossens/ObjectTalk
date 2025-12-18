//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtLog.h"

#include "OtRenderPass.h"
#include "OtRenderPipeline.h"
#include "OtSampler.h"
#include "OtTexture.h"

#include "OtFullScreenVert.h"
#include "OtCompositingFrag.h"


//
//	OtCompositing
//

class OtCompositing {
public:
	// destructor
	virtual inline ~OtCompositing() {}

	// clear GPU resources
	virtual inline void clear() {
		renderPipeline.clear();
		sampler.clear();
	}

	// set properties
	inline void setBrightness(float value) { brightness = value; }

	// composite input on top of output
	void render(OtTexture& source, OtTexture& destination) {
		// sanity check
		if (!source.canBeSampled()) {
			OtLogFatal("Input texture to compositing does not have [sampler] usage");
		}

		if (!destination.isColorTarget()) {
			OtLogFatal("Output texture to compositing is not a render target");
		}

		// create pipeline (if required)
		if (!renderPipeline.isValid()) {
			// determine render target type
			OtRenderPipeline::RenderTargetType renderTargetType = OtRenderPipeline::RenderTargetType::none;

			switch (destination.getFormat()) {
				case OtTexture::Format::r8: renderTargetType = OtRenderPipeline::RenderTargetType::r8; break;
				case OtTexture::Format::rg16: renderTargetType = OtRenderPipeline::RenderTargetType::rg16; break;
				case OtTexture::Format::rgba8: renderTargetType = OtRenderPipeline::RenderTargetType::rgba8; break;
				case OtTexture::Format::rgba16: renderTargetType = OtRenderPipeline::RenderTargetType::rgba16; break;
				case OtTexture::Format::rgba32: renderTargetType = OtRenderPipeline::RenderTargetType::rgba32; break;
				default: OtLogFatal("Unsupported render target type");
			}

			renderPipeline.setShaders(OtFullScreenVert, sizeof(OtFullScreenVert), OtCompositingFrag, sizeof(OtCompositingFrag));
			renderPipeline.setRenderTargetType(renderTargetType);
			configurePipeline(renderPipeline);
		}

		// setup uniforms
		struct Uniforms {
			float brightness;
		} uniforms {
			brightness
		};

		// configure pass
		OtRenderPass pass;
		pass.start(destination);
		pass.bindPipeline(renderPipeline);
		pass.bindFragmentSampler(0, sampler, source);
		pass.bindFragmentUniforms(0, &uniforms, sizeof(uniforms));
		pass.render(3);
		pass.end();
	}

protected:
	// methods to be overridden by derived classes (if required)
	virtual void configurePipeline([[maybe_unused]] OtRenderPipeline& pipeline) {}
	virtual void configurePass([[maybe_unused]] OtRenderPass& pass) {}

	// the rendering pipeline
	OtRenderPipeline renderPipeline;

	// work variables
	OtSampler sampler{OtSampler::Filter::nearest, OtSampler::Addressing::clamp};
	float brightness = 1.0f;
};
