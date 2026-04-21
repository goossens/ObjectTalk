//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtSingleton.h"


//
//	OtConfig
//

class OtConfig : OtSingleton<OtConfig> {
public:
	// access subprocess mode
	static inline void setSubprocessMode(bool flag) { instance().subprocessMode = flag; }
	static inline bool inSubprocessMode() { return instance().subprocessMode; }

	// access forced vulkan backend
	static inline void setForcedVulkanBackend(bool flag) { instance().forcedVulkanBackend = flag; }
	static inline bool isForcedVulkanBackend() { return instance().forcedVulkanBackend; }

private:
	// configuration
	bool subprocessMode = false;
	bool forcedVulkanBackend = false;
};
