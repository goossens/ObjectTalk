//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtImageCanvas.h"


//
//	OtImageCanvas::OtImageCanvas
//

OtImageCanvas::OtImageCanvas(int w, int h) {
	// create the rendering surface and canvas
	surface = plutovg_surface_create(w, h);
	canvas = plutovg_canvas_create(surface);
}


//
//	OtImageCanvas::~OtImageCanvas
//

OtImageCanvas::~OtImageCanvas() {
	// cleanup
	plutovg_canvas_destroy(canvas);
	plutovg_surface_destroy(surface);
}


//
//	OtImageCanvas::render
//

void OtImageCanvas::render(OtImage& image) {
	// convert ABGR to RGBA
	int w = plutovg_surface_get_width(surface);
	int h = plutovg_surface_get_height(surface);
	image.update(w, h, OtImage::Format::rgba8);

	auto src = (uint32_t*) plutovg_surface_get_data(surface);
	auto dest = (unsigned char*) image.getPixels();

	for (auto y = 0; y < h; y++) {
		for (auto x = 0; x < w; x++) {
			auto pixel = *src++;
			uint32_t a = (pixel >> 24) & 0xFF;

			if (a == 0) {
				*dest++ = 0;
				*dest++ = 0;
				*dest++ = 0;
				*dest++ = 0;

			} else {
				uint32_t r = (pixel >> 16) & 0xFF;
				uint32_t g = (pixel >> 8) & 0xFF;
				uint32_t b = (pixel >> 0) & 0xFF;

				if (a != 255) {
					r = (r * 255) / a;
					g = (g * 255) / a;
					b = (b * 255) / a;
				}

				*dest++ = static_cast<unsigned char>(r);
				*dest++ = static_cast<unsigned char>(g);
				*dest++ = static_cast<unsigned char>(b);
				*dest++ = static_cast<unsigned char>(a);
			}
		}
	}

	image.incrementVersion();
}
