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
#include <functional>

#include "OtBgfxHandle.h"
#include "OtImage.h"
#include "OtTexture.h"


//
//	OtReadBackBuffer
//

class OtReadBackBuffer {
public:
	// clear the data
	void clear();

	// readback the specified texture
	void readback(OtTexture& texture, std::function<void()> callback=nullptr);

	// get properties
	inline OtImage& getImage() { return image; }

private:
	// properties
	OtBgfxHandle<bgfx::TextureHandle> readbackTexture;
	OtImage image;
	int width = 0;
	int height = 0;
	int format = OtTexture::noTexture;
};
