//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtCompositing.h"


//
//	OtCompositingAdd
//

class OtCompositingAdd : public OtCompositing {
protected:
	void configurePipeline(OtRenderPipeline& pipeline) override {
		pipeline.setBlend(
			OtRenderPipeline::BlendOperation::add,
			OtRenderPipeline::BlendFactor::one,
			OtRenderPipeline::BlendFactor::one
		);
	}
};
