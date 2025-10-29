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
#include "OtPass.h"


//
//	OtCompositingAlphaOver
//

class OtCompositingAlphaOver : public OtCompositing {
private:
	// get render state
	uint64_t getState() override {
		return
			OtPass::stateWriteRgb |
			OtPass::stateWriteA |
			OtPass::stateBlendAlpha;
	}
};
