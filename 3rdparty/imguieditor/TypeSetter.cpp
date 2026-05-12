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
//	TextEditor::TypeSetter::wrapLine
//

void TextEditor::TypeSetter::wrapLine(Line& line) {
	// there is always one visible row for a line
	line.rows = 1;

	// status for wrapped lines
	LineSections sections;

	// track breaks
	size_t lastBreakIndex = 0;
	size_t lastBreakableIndex = 0;
	size_t lastBreakableColumn = 0;
	size_t columns = 0;
	size_t indent = 0;

	// track first row indent
	bool isAtBeginning = true;
	bool isFirstRow = true;

	// process all glyphs on line
	size_t i = 0;
	size_t size = line.size();

	while (i < size) {
		// handle hard break
		if (line[i].breakOption == BreakOption::mustBreak) {
			// this can't happen in the editor as hard breaks have already been handled
			// code is however left here in case we want to reuse it somewhere else later
			line.rows++;
			sections.emplace_back(lastBreakIndex, i, columns, indent);

			columns = 0;
			indent = 0;
			isAtBeginning = true;
			isFirstRow = true;
			lastBreakIndex = i + 1;
			lastBreakableIndex = i + 1;

		} else {
			// get current codepoint
			auto codepoint = line[i].codepoint;

			// calculate first row indent (if required)
			if (isAtBeginning) {
				if (CodePoint::isWhiteSpace(codepoint)) {
					indent = (codepoint == '\t') ? ((indent / tabSize) + 1) * tabSize : indent + 1;

				} else {
					isAtBeginning = false;
				}
			}

			// update column count
			columns = (codepoint == '\t') ? ((columns / tabSize) + 1) * tabSize : columns + 1;

			if (columns < wordWrapColumns) {
				// we're not at the end of the row yet so we have to track any break options
				if (line[i].breakOption == BreakOption::allowBreak) {
					lastBreakableIndex = i + 1;
					lastBreakableColumn = columns;
				}

				i++;

			} else {
				// we have reached the end of the row
				if (lastBreakableIndex == lastBreakIndex) {
					// no breaks found, just truncate here
					lastBreakableIndex = i + 1;
					sections.emplace_back(lastBreakIndex, lastBreakableIndex, columns, isFirstRow ? 0 : indent);

				} else {
					// break at last breakable position
					sections.emplace_back(lastBreakIndex, lastBreakableIndex, lastBreakableColumn, isFirstRow ? 0 : indent);
				}

				i = lastBreakableIndex;

				// start a new row
				line.rows++;
				columns = indent;
				lastBreakIndex = lastBreakableIndex;
				lastBreakableColumn = indent;
				isFirstRow = false;
			}
		}
	}

	// did we end up with line wrapping?
	if (line.rows > 1) {
		// yes, save wrapping information
		sections.emplace_back(lastBreakIndex, size, columns, indent);
		line.sections = std::make_shared<LineSections>(sections);

		line.columns = 0;

		for (auto& section : sections) {
			line.columns = std::max(line.columns, section.columns);
		}

	} else {
		// no wrapping
		line.sections = nullptr;
		line.columns = columns;
	}
}


//
//	TextEditor::TypeSetter::updateLine
//

void TextEditor::TypeSetter::updateLine(Line& line) {
	if (wordWrap) {
		// classify all line break opportunities in line
		lineBreak.classify(line);

		// determine number of rows required
		wrapLine(line);

	} else {
		// text is always 1 row high without wrapping
		line.rows = 1;

		// determine the maximum column number for this line
		line.columns = 0;

		for (auto& glyph : line) {
			line.columns = (glyph.codepoint == '\t') ? ((line.columns / tabSize) + 1) * tabSize : line.columns + 1;
		}

		// reset multiline sections
		line.sections = nullptr;
	}

	line.needsTypeSetting = false;
}


//
//	TextEditor::TypeSetter::update
//

bool TextEditor::TypeSetter::update(const Config& config, Document& document, LineFold& lineFold) {
	// see if the configuration changed
	bool configChanged =
		tabSize != config.tabSize ||
		wordWrap != config.wordWrap ||
		(wordWrap && wordWrapColumns != config.wordWrapColumns);

	// update all lines on configuration changes
	if (configChanged) {
		// capture relevant configuration so we can detect new changes later
		tabSize = config.tabSize;
		wordWrap = config.wordWrap;
		wordWrapColumns = config.wordWrapColumns;

		// update all the lines
		for (auto& line : document) {
			updateLine(line);
		}

	// update changed lines when document is updated
	} else if (document.isUpdated()) {
		for (auto& line : document) {
			if (line.needsTypeSetting) {
				updateLine(line);
			}
		}
	}

	// update overall state if something changed
	updated = configChanged || document.isUpdated() || lineFold.isUpdated();

	if (updated) {
		// reset state
		clear();
		totalRows = 0;
		totalColumns = 0;

		// process all document lines
		size_t lineNo = 0;

		for (auto& line : document) {
			// update line's row start
			// this is even required for hidden lines so their DocPos can be transformed into a VisPos
			line.row = totalRows;

			// skip hidden lines
			if (line.foldingState != FoldingState::hidden) {
				// update total rows and columns
				totalRows += line.rows;
				totalColumns = std::max(totalColumns, line.columns);

				// add row(s) and handle word wrapping (if required)
				if (line.sections) {
					for (size_t i = 0; i < line.rows; i++) {
						emplace_back(lineNo, i, line.sections->at(i).columns);
					}

				} else {
					emplace_back(lineNo, 0, line.columns);
				}
			}

			lineNo++;
		}
	}

	// update and return status
	return updated;
}


//
//	TextEditor::TypeSetter::docPos2VisPos
//

TextEditor::VisPos TextEditor::TypeSetter::docPos2VisPos(const Document& document, DocPos pos) const {
	auto& line = document[pos.line];
	VisPos visPos(line.row, 0);

	if (line.foldingState == FoldingState::hidden) {
		// for hidden lines, we use the position of the ellipsis (end of folded line)
		visPos.column = at(line.row).columns;

	} else if (line.sections) {
		// for wrapped lines, find relevant section and handle tabs
		bool done = false;

		for (size_t i = 0; !done && i < line.sections->size(); i++) {
			auto& section = line.sections->at(i);

			if (pos.index >= section.startIndex && pos.index <= section.endIndex) {
				auto start = line.begin() + section.startIndex;
				auto end = line.begin() + pos.index;
				visPos.column = section.indent;

				for (auto glyph = start; glyph < end; glyph++) {
					visPos.column = (glyph->codepoint == '\t') ? ((visPos.column / tabSize) + 1) * tabSize : visPos.column + 1;
				}

				done = true;

			} else {
				visPos.row++;
			}
		}

	} else {
		// for non-wrapped lines, just handle tabs
		auto end = line.begin() + pos.index;

		for (auto glyph = line.begin(); glyph < end; glyph++) {
			visPos.column = (glyph->codepoint == '\t') ? ((visPos.column / tabSize) + 1) * tabSize : visPos.column + 1;
		}
	}

	return visPos;
}


//
//	TextEditor::TypeSetter::visPos2DocPos
//

TextEditor::DocPos TextEditor::TypeSetter::visPos2DocPos(const Document& document, VisPos pos) const {
	if (pos.row >= size()) {
		return DocPos(0, 0);
	}

	auto& row = at(pos.row);
	auto& line = document[row.line];

	DocPos docPos;
	docPos.line = row.line;

	size_t index;
	size_t leftColumn;
	size_t rightColumn;

	Line::const_iterator start;
	Line::const_iterator end;

	if (line.sections) {
		auto& section = line.sections->at(row.section);
		index = section.startIndex;
		leftColumn = section.indent;
		rightColumn = section.indent;

		start = line.begin() + section.startIndex;
		end = line.begin() + section.endIndex;

	} else {
		index = 0;
		leftColumn = 0;
		rightColumn = 0;

		start = line.begin();
		end = line.end();
	}

	for (auto glyph = start; rightColumn < pos.column && glyph < end; glyph++) {
		leftColumn = rightColumn;
		rightColumn = (glyph->codepoint == '\t') ? ((rightColumn / tabSize) + 1) * tabSize : rightColumn + 1;
		index++;
	}

	if (rightColumn - leftColumn <= 1) {
		docPos.index = index;

	} else {
		auto leftDiff = pos.column - leftColumn;
		auto rightDiff = rightColumn - pos.column;
		docPos.index = leftDiff <= rightDiff ? index - 1 : index;
	}

	return docPos;
}


//
//	TextEditor::TypeSetter::screenPos2DocPos
//

void TextEditor::TypeSetter::screenPos2DocPos(const Document& document, ImVec2 screenPos, DocPos& glyphPos, DocPos& cursorPos) const {
	// the returned glyphPos addresses the glyph pointed to by the screenPos parameter (row and column in floating point format)
	// the returned cursorPos returns the closest cursor position (which can be at the start or the end of the glyph)
	size_t colNo = static_cast<size_t>(screenPos.x);
	size_t rowNo = static_cast<size_t>(screenPos.y);

	if (screenPos.y <= 0.0f) {
		glyphPos = DocPos(0, 0);
		cursorPos = glyphPos;

	} else if (rowNo >= totalRows) {
		glyphPos = document.getBottom();
		cursorPos = glyphPos;

	} else {
		auto& row = at(rowNo);
		auto& line = document[row.line];

		if (screenPos.x <= 0.0f) {
			glyphPos = DocPos(row.line, 0);
			cursorPos = glyphPos;

		} else if (colNo > row.columns) {
			if (line.sections) {
				auto& section = line.sections->at(row.section);
				glyphPos = DocPos(row.line, section.endIndex);

			} else {
				glyphPos = document.getEndOfLine(DocPos(row.line, 0));
			}

			cursorPos = glyphPos;

		} else {
			size_t index;
			size_t leftColumn;
			size_t rightColumn;

			Line::const_iterator start;
			Line::const_iterator end;

			if (line.sections) {
				auto& section = line.sections->at(row.section);
				index = section.startIndex;
				leftColumn = section.indent;
				rightColumn = section.indent;

				start = line.begin() + section.startIndex;
				end = line.begin() + section.endIndex;

			} else {
				index = 0;
				leftColumn = 0;
				rightColumn = 0;

				start = line.begin();
				end = line.end();

			}

			for (auto glyph = start; static_cast<float>(rightColumn) < screenPos.x && glyph < end; glyph++) {
				leftColumn = rightColumn;
				rightColumn = (glyph->codepoint == '\t') ? ((rightColumn / tabSize) + 1) * tabSize : rightColumn + 1;
				index++;
			}

			auto leftDiff = screenPos.x - static_cast<float>(leftColumn);
			auto rightDiff = static_cast<float>(rightColumn) - screenPos.x;

			glyphPos = DocPos(row.line, index - 1);
			cursorPos = DocPos(row.line, leftDiff <= rightDiff ? index - 1 : index);
		}
	}
}


//
//	TextEditor::TypeSetter::normalizePos
//

TextEditor::VisPos TextEditor::TypeSetter::normalizePos(VisPos pos) const {
	if (totalRows == 0) {
		return VisPos(0, 0);

	} else if (pos.row >= totalRows) {
		auto lastRow = totalRows - 1;
		return VisPos(lastRow, at(lastRow).columns);

	} else if (pos.column > at(pos.row).columns) {
		return VisPos(pos.row, at(pos.row).columns);

	} else {
		return pos;
	}
}


//
//	TextEditor::TypeSetter::isVisPosOverGlyph
//

bool TextEditor::TypeSetter::isVisPosOverGlyph(VisPos pos) const {
	return pos.row < totalRows && pos.column <= at(pos.row).columns;
}
