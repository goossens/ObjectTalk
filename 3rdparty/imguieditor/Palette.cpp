//	TextEditor - A syntax highlighting text editor for Dear ImGui.
//	Copyright (c) 2024-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "TextEditor.h"


//
//	TextEditor::updatePalette
//

void TextEditor::updatePalettes() {
	// Update palette with the current alpha from the Dear ImGui style
	paletteAlpha = ImGui::GetStyle().Alpha;

	for (size_t i = 0; i < static_cast<size_t>(Color::count); i++) {
		auto color = ImGui::ColorConvertU32ToFloat4(paletteBase[i]);

		color.w *= paletteAlpha;
		palette[i] = ImGui::ColorConvertFloat4ToU32(color);

		color.w *= miniMapAlpha;
		miniMapPalette[i] = ImGui::ColorConvertFloat4ToU32(color);
	}
}


//
//	Color palettes
//

const TextEditor::Palette& TextEditor::GetDarkPalette() {
	const static Palette palette = {{
		IM_COL32(224, 224, 224, 255),	// text
		IM_COL32(197, 134, 192, 255),	// keyword
		IM_COL32( 90, 179, 155, 255),	// declaration
		IM_COL32(181, 206, 168, 255),	// number
		IM_COL32(206, 145, 120, 255),	// string
		IM_COL32(255, 255, 153, 255),	// punctuation
		IM_COL32( 64, 192, 128, 255),	// preprocessor
		IM_COL32(156, 220, 254, 255),	// identifier
		IM_COL32( 79, 193, 255, 255),	// known identifier
		IM_COL32(106, 153,  85, 255),	// comment
		IM_COL32( 30,  30,  30, 255),	// background
		IM_COL32(224, 224, 224, 255),	// cursor
		IM_COL32( 32,  96, 160, 255),	// selection
		IM_COL32( 80,  80,  80, 255),	// whitespace
		IM_COL32( 70,  70,  70, 255),	// matchingBracketBackground
		IM_COL32(140, 140, 140, 255),	// matchingBracketActive
		IM_COL32(246, 222,  36, 255),	// matchingBracketLevel1
		IM_COL32( 66, 120, 198, 255),	// matchingBracketLevel2
		IM_COL32(213,  96, 213, 255),	// matchingBracketLevel3
		IM_COL32(198,   8,  32, 255),	// matchingBracketError
		IM_COL32(128, 128, 144, 255),	// line number
		IM_COL32(224, 224, 240, 255),	// current line number
	}};

	return palette;
}

const TextEditor::Palette& TextEditor::GetLightPalette() {
	const static Palette palette = {{
		IM_COL32( 64,  64,  64, 255),	// text
		IM_COL32( 170,  0, 220, 255),	// keyword
		IM_COL32( 65,   0, 255, 255),	// declaration
		IM_COL32( 40, 140,  90, 255),	// number
		IM_COL32(160,  32,  32, 255),	// string
		IM_COL32(  0,   0,   0, 255),	// punctuation
		IM_COL32( 96,  96,  64, 255),	// preprocessor
		IM_COL32( 64,  64,  64, 255),	// identifier
		IM_COL32( 16,  96,  96, 255),	// known identifier
		IM_COL32( 35, 135,   5, 255),	// comment
		IM_COL32(255, 255, 255, 255),	// background
		IM_COL32(  0,   0,   0, 255),	// cursor
		IM_COL32(  0,   0,  96,  64),	// selection
		IM_COL32(144, 144, 144, 144),	// whitespace
		IM_COL32(180, 180, 180, 144),	// matchingBracketBackground
		IM_COL32( 72,  72,  72, 255),	// matchingBracketActive
		IM_COL32( 70,   0, 250, 255),	// matchingBracketLevel1
		IM_COL32( 80, 160,  70, 255),	// matchingBracketLevel2
		IM_COL32(120,  60,  25, 255),	// matchingBracketLevel3
		IM_COL32(198,   8,  32, 255),	// matchingBracketError
		IM_COL32(  0,  80,  80, 255),	// line number
		IM_COL32(  0,   0,   0, 255),	// current line number
	}};

	return palette;
}

TextEditor::Palette TextEditor::defaultPalette = TextEditor::GetDarkPalette();
