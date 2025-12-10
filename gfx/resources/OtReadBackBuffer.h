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

#include "OtImage.h"
#include "OtTexture.h"


//
//	OtReadBackBuffer
//

class OtReadBackBuffer {
public:
	// clear the data
	void clear();

	// read back the specified texture
	OtImage& readback(OtTexture& texture);

	// read back part of the specified texture
	OtImage& readback(OtTexture& texture, int x, int y, int w, int h);

	// get properties
	inline OtImage& getImage() { return image; }

private:
	// properties
	OtImage image;

	// convert downloaded texture data to image
	void convertToImage(int w, int h, OtTexture::Format format, void* data);
};
