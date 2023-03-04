//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bx/file.h"

#include "OtCodePoint.h"
#include "OtException.h"
#include "OtFunction.h"

#include "OtFont.h"


//
//	Instantiate truetype code
//

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "imstb_truetype.h"


//
//	OtFontClass::~OtFontClass
//

OtFontClass::~OtFontClass() {
	// detach from noisemap if required
	if (data) {
		delete [] data;
	}
}


//
//	OtFontClass::init
//

void OtFontClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 2:
			setSize(parameters[1]->operator float());

		case 1:
			setFont(parameters[0]->operator std::string());

		case 0:
			break;

		default:
			OtExcept("[Font] constructor expects up to 2 arguments (not %ld)", count);
	}
}


//
//	OtFontClass::setFont
//

OtObject OtFontClass::setFont(const std::string& file) {
	// cleanup if required
	if (data) {
		delete [] data;
		data = nullptr;
	}
	// load font file into memory
	static bx::FileReader reader;

	if (!bx::open(&reader, file.c_str())) {
		OtExcept("Can't open font [%s]", file.c_str());
	}

	uint32_t size = (uint32_t) bx::getSize(&reader);
	data = new uint8_t[size];
	bx::read(&reader, data, size, bx::ErrorAssert{});
	bx::close(&reader);

	// prepare font
	if (!stbtt_InitFont(&font, data, 0)) {
		OtExcept("Can't process font [%s]", file.c_str());
	}

	return OtObject(this);
}


//
//	OtFontClass::setSize
//

OtObject OtFontClass::setSize(float s) {
	size = s;
	return OtObject(this);
}


//
//	OtFontClass::getWidth
//

float OtFontClass::getWidth(const std::string& text) {
	// process all unicode characters (codepoints)
	int width = 0;
	auto i = text.begin();

	while (i < text.end()) {
		int32_t codepoint;
		i = OtCodePoint::get(i, &codepoint);

		// get character width
		int advanceWidth;
		int leftSideBearing;
		stbtt_GetCodepointHMetrics(&font, codepoint, &advanceWidth, &leftSideBearing);
		width += advanceWidth;
	}

	// scale result
	return stbtt_ScaleForPixelHeight(&font, size) * (float) width;
}


//
//	OtFontClass::createShape
//

void OtFontClass::createShape(OtShape shape, const std::string& text) {
	// horizontal character offset
	int offset = 0;

	// determine scaling factor
	float scale = stbtt_ScaleForPixelHeight(&font, size);

	// process all unicode characters (codepoints)
	auto i = text.begin();

	while (i < text.end()) {
		int32_t codepoint;
		i = OtCodePoint::get(i, &codepoint);

		// get character width
		int advanceWidth;
		int leftSideBearing;
		stbtt_GetCodepointHMetrics(&font, codepoint, &advanceWidth, &leftSideBearing);

#define X(x) ((x + offset) * scale)
#define Y(y) (y * scale)

		// process all vertices (drawing commands) for this codepoint
		stbtt_vertex* verts;
		int count = stbtt_GetCodepointShape(&font, codepoint, &verts);

		for (auto c = 0; c < count; c++) {
			switch (verts[c].type) {
				case STBTT_vmove:
					shape->moveTo(X(verts[c].x), Y(verts[c].y));
					break;

				case STBTT_vline:
					shape->lineTo(X(verts[c].x), Y(verts[c].y));
					break;

				case STBTT_vcurve:
					shape->quadraticCurveTo(X(verts[c].cx), Y(verts[c].cy), X(verts[c].x), Y(verts[c].y));
					break;

				case STBTT_vcubic:
					shape->bezierCurveTo(X(verts[c].cx), Y(verts[c].cy), X(verts[c].cx1), Y(verts[c].cy1), X(verts[c].x), Y(verts[c].y));
					break;
			}
		}

		// advance the charector offset
		offset += advanceWidth;
	}

#undef X
#undef Y
}


//
//	OtFontClass::getMeta
//

OtType OtFontClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtFontClass>("Font", OtGuiClass::getMeta());
		type->set("__init__", OtFunction::create(&OtFontClass::init));

		type->set("setFont", OtFunction::create(&OtFontClass::setFont));
		type->set("setSize", OtFunction::create(&OtFontClass::setSize));

		type->set("getWidth", OtFunction::create(&OtFontClass::getWidth));
	}

	return type;
}
