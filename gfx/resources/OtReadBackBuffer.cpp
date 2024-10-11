//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bimg/bimg.h"

#include "OtFrameworkAtFrame.h"
#include "OtPass.h"
#include "OtReadBackBuffer.h"


//
//	OtReadBackBuffer::clear
//

void OtReadBackBuffer::clear() {
	// clear the texture and image
	readbackTexture.clear();
	image.clear();
}


//
//	OtReadBackBuffer::readback
//

void OtReadBackBuffer::readback(OtTexture& texture, std::function<void()> callback) {
	// update texture and buffer if required
	if (!readbackTexture.isValid() || width != texture.getWidth() || height != texture.getHeight() || format != texture.getFormat()) {
		// remember new specs
		width = texture.getWidth();
		height = texture.getHeight();
		format = texture.getFormat();

		// create new texture
		readbackTexture = bgfx::createTexture2D(
			uint16_t(width),
			uint16_t(height),
			false,
			1,
			bgfx::TextureFormat::Enum(format),
			BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK);

		image.update(texture.getWidth(), texture.getHeight(), texture.getFormat());
	}

	// blit source texture to readback texture on GPU
	OtPass pass;

	pass.blit(
		readbackTexture.getHandle(),
		0, 0,
		texture.getHandle(),
		0, 0, uint16_t(texture.getWidth()), uint16_t(texture.getHeight()));

	// request readback to CPU
	auto frame = bgfx::readTexture(readbackTexture.getHandle(), image.getPixels());

	// it takes 2 frames before the readback is complete so we increment the version in a callback
	OtFrameworkAtFrame::add(frame, [this, callback]() {
		image.incrementVersion();

		if (callback) {
			callback();
		}
	});
}
