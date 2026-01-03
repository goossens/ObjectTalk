//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "imstb_truetype.h"


//
//	OtFont
//

class OtFont {
public:
	// constructors/destructor
	OtFont() = default;
	OtFont(const std::string& path) { load(path); }

	// clear the resources
	inline void clear() { fontData.reset(); }

	// see if font is ready to go
	inline bool isValid() { return fontData != nullptr; }

	// load the font
	void load(const std::string& path);

	// see if font contains codepoint
	bool hasCodePoint(int32_t codepoint);

	// get the width of a text string
	float getWidth(const std::string& text, float size);

	// parse glyphs and call functions for outline
	void parseGlyphs(
		const std::string& text,
		float size,
		std::function<void(float, float)> moveTo,
		std::function<void(float, float)> lineTo,
		std::function<void(float, float, float, float)> quadraticCurveTo,
		std::function<void(float, float, float, float, float, float)> bezierCurveTo);

	// see if fonts are identical
	inline bool operator==(OtFont& rhs) {
		return fontData == rhs.fontData;
	}

	inline bool operator!=(OtFont& rhs) {
		return !operator==(rhs);
	}

private:
	// font data
	struct FontData{
		FontData(size_t size) { bytes.resize(size); }
		std::vector<uint8_t> bytes;
		stbtt_fontinfo font;
	};

	std::shared_ptr<FontData> fontData;
};
