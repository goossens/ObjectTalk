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
//	TextEditor::MiniMap::update
//

bool TextEditor::MiniMap::update(const Config& config, const Document& document, const TypeSetter& typeSetter) {
	// update all lines on configuration change
	bool configChanged = showMiniMap != config.showMiniMap;

	if (configChanged) {
		showMiniMap = config.showMiniMap;
	}

	if (configChanged || typeSetter.isUpdated()) {
		// reset state
		auto rowCount = typeSetter.getRowCount();
		rows.clear();

		// process all rows
		for (size_t i = 0; i < rowCount; i++) {
			auto& line = document[typeSetter[i].line];

			if (line.foldingState != FoldingState::hidden) {
				size_t index;
				size_t column;
				size_t endColumn;

				// determine visible part of document line on row
				if (config.wordWrap && line.sections) {
					auto& section = line.sections->at(typeSetter[i].section);
					index = section.startIndex;
					column = section.indent;
					endColumn = section.columns;

				} else {
					index = 0;
					column = 0;
					endColumn = line.columns;
				}

				// process line
				processLine(line, config, index, column, endColumn);
			}
		}
	}

	// return status
	return configChanged || typeSetter.isUpdated();
}


//
//	TextEditor::MiniMap::processLine
//

void TextEditor::MiniMap::processLine(
	const Line& line,
	const Config& config,
	size_t index,
	size_t column,
	size_t endColumn) {

	auto& row = rows.emplace_back();
	auto start = column;
	auto color = Color::background;

	// process all
	while (column < endColumn) {
		auto& glyph = line[index++];

		// detect end of section
		if (glyph.color != color) {
			if (column != start) {
				if (color != Color::whitespace) {
					row.sections.emplace_back(start, column, color);
				}

				start = column;
			}

			color = glyph.color;
		}

		// update column number
		column += (glyph.codepoint == '\t') ? (config.tabSize - (column % config.tabSize)) : 1;
	}

	// handle possible sections at the end of the row
	if (column != start && color != Color::whitespace) {
		row.sections.emplace_back(start, column, color);
	}
}
