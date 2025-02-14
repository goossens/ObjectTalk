//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <fstream>

#include "OtAssert.h"
#include "OtCodePoint.h"

#include "OtFont.h"
#include "OtPath.h"


//
//	Instantiate truetype code
//

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "imstb_truetype.h"


//
//	OtFont::load
//

void OtFont::load(const std::string& path) {
	// basic housekeeping
	clear();

	// load font file into memory
	if (!OtPath::exists(path) || !OtPath::isRegularFile(path)) {
		OtError("Can't open font in [{}]", path);
	}

	auto filesize = OtPath::getFileSize(path);
	fontdata = std::make_shared<FontData>(filesize);
	std::ifstream stream(path.c_str(), std::ios::binary);
	stream.read((char*) fontdata->bytes.data(), filesize);

	if (!stream) {
		fontdata.reset();
		OtError("Can't open font in [{}]", path);
	}

	// prepare font
	if (!stbtt_InitFont(&fontdata->font, fontdata->bytes.data(), 0)) {
		fontdata.reset();
		OtError("Can't process font [{}]", path);
	}
}


//
//	OtFont::hasCodePoint
//

bool OtFont::hasCodePoint(int32_t codepoint) {
	OtAssert(fontdata);
	return stbtt_FindGlyphIndex(&fontdata->font, codepoint) != 0;
}


//
//	OtFont::getWidth
//

float OtFont::getWidth(const std::string& text, float size) {
	// sanity check
	OtAssert(fontdata);

	// process all unicode characters (codepoints)
	int width = 0;
	auto end = text.end();
	auto i = text.begin();

	while (i < end) {
		char32_t codepoint;
		i = OtCodePoint::read(i, end, &codepoint);

		// get character width
		int advanceWidth;
		int leftSideBearing;
		stbtt_GetCodepointHMetrics(&fontdata->font, codepoint, &advanceWidth, &leftSideBearing);
		width += advanceWidth;
	}

	// scale result
	return stbtt_ScaleForPixelHeight(&fontdata->font, size) * (float) width;
}


//
//	OtFont::parseGlyphs
//

void OtFont::parseGlyphs(
	const std::string& text,
	float size,
	std::function<void(float, float)> moveTo,
	std::function<void(float, float)> lineTo,
	std::function<void(float, float, float, float)> quadraticCurveTo,
	std::function<void(float, float, float, float, float, float)> bezierCurveTo) {

#define X(x) ((x + offset) * scale)
#define Y(y) (y * scale)

	// sanity check
	OtAssert(fontdata);

	// horizontal character offset
	int offset = 0;

	// determine scaling factor
	float scale = stbtt_ScaleForPixelHeight(&fontdata->font, size);

	// process all unicode characters (codepoints)
	auto end = text.end();
	auto i = text.begin();

	while (i < end) {
		char32_t codepoint;
		i = OtCodePoint::read(i, end, &codepoint);

		// process all vertices (drawing commands) for this codepoint
		stbtt_vertex* verts;
		int count = stbtt_GetCodepointShape(&fontdata->font, codepoint, &verts);

		for (auto c = 0; c < count; c++) {
			switch (verts[c].type) {
				case STBTT_vmove:
					moveTo(X(verts[c].x), Y(verts[c].y));
					break;

				case STBTT_vline:
					lineTo(X(verts[c].x), Y(verts[c].y));
					break;

				case STBTT_vcurve:
					quadraticCurveTo(X(verts[c].cx), Y(verts[c].cy), X(verts[c].x), Y(verts[c].y));
					break;

				case STBTT_vcubic:
					bezierCurveTo(X(verts[c].cx), Y(verts[c].cy), X(verts[c].cx1), Y(verts[c].cy1), X(verts[c].x), Y(verts[c].y));
					break;
			}
		}

		// advance the charector offset
		int advanceWidth;
		int leftSideBearing;
		stbtt_GetCodepointHMetrics(&fontdata->font, codepoint, &advanceWidth, &leftSideBearing);
		offset += advanceWidth;
	}

#undef X
#undef Y
}
