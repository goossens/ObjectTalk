//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtCompositing.h"


//
//	OtCompositingAlphaOver
//

class OtCompositingAlphaOver : public OtCompositing {
protected:
	void configurePipeline(OtRenderPipeline& pipeline) override {
		pipeline.setBlend(
			OtRenderPipeline::BlendOperation::add,
			OtRenderPipeline::BlendFactor::srcAlpha,
			OtRenderPipeline::BlendFactor::oneMinusSrcAlpha
		);
	}
};
