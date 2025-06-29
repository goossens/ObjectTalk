//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "imgui.h"

#include "OtTexture.h"


//
//	OtLogo
//

class OtLogo {
public:
	// get logo information
	ImTextureID getTextureID();

	// get logo size
	inline size_t getWidth() { return 429; }
	inline size_t getHeight() { return 350; }
	inline ImVec2 getSize() { return ImVec2(static_cast<float>(getWidth()), static_cast<float>(getHeight())); }

private:
	// the logo as a texture
	OtTexture texture;
};
