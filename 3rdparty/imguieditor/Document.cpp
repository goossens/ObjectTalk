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
//	TextEditor::Document::setText
//

void TextEditor::Document::setText(const std::string_view& text) {
	// reset document
	clearDocument();
	appendLine();
	updated = true;

	// process UTF-8 and generate lines of glyphs
	auto end = text.end();
	auto i = CodePoint::skipBOM(text.begin(), end);

	while (i < end) {
		ImWchar character;
		i = CodePoint::read(i, end, &character);

		if (character == '\n') {
			appendLine();

		} else if (insertSpacesOnTabs && character == '\t') {
			auto spaces = ((back().size() / tabSize) + 1) * tabSize - back().size();

			for (size_t s = 0; s < spaces; s++) {
				back().emplace_back(Glyph(' ', Color::text));
			}

		} else if (character != '\r') {
			back().emplace_back(Glyph(character, Color::text));
		}
	}

	// update maximum column counts
	updateMaximumColumn(0, lineCount() - 1);
}


//
//	TextEditor::Document::setText
//

void TextEditor::Document::setText(const std::vector<std::string_view>& text) {
	// reset document
	clearDocument();
	updated = true;

	if (text.size()) {
		// process input UTF-8 and generate lines of glyphs
		for (auto& line : text) {
			appendLine();
			auto i = line.begin();
			auto end = line.end();

			while (i < end) {
				ImWchar character;
				i = CodePoint::read(i, end, &character);

				if (insertSpacesOnTabs && character == '\t') {
					auto spaces = ((back().size() / tabSize) + 1) * tabSize - back().size();

					for (size_t s = 0; s < spaces; s++) {
						back().emplace_back(Glyph(' ', Color::text));
					}

				} else if (character != '\r') {
					back().emplace_back(Glyph(character, Color::text));
				}
			}
		}

	} else {
		appendLine();
	}

	// update maximum column counts
	updateMaximumColumn(0, lineCount() - 1);
}


//
//	TextEditor::Document::insertText
//

TextEditor::Coordinate TextEditor::Document::insertText(Coordinate start, const std::string_view& text) {
	auto line = begin() + start.line;
	auto index = getIndex(start);
	auto lineNo = start.line;

	// process input as UTF-8
	auto endOfText = text.end();
	auto i = text.begin();

	// process all codepoints
	while (i < endOfText) {
		ImWchar character;
		i = CodePoint::read(i, endOfText, &character);

		if (character == '\n') {
			// split line
			insertLine(lineNo + 1);
			line = begin() + lineNo;
			auto nextLine = begin() + ++lineNo;

			for (auto j = line->begin() + index; j < line->end(); j++) {
				nextLine->push_back(*j);
			}

			line->erase(line->begin() + index, line->end());
			line = nextLine;
			index = 0;

		} else if (insertSpacesOnTabs && character == '\t') {
			auto spaces = ((index / tabSize) + 1) * tabSize - index;

			for (size_t s = 0; s < spaces; s++) {
				line->insert(line->begin() + (index++), Glyph(' ', Color::text));
			}

		} else if (character != '\r') {
			// insert next glyph
			line->insert(line->begin() + (index++), Glyph(character, Color::text));
		}
	}

	// determine end of insert
	auto end = Coordinate(lineNo, getColumn(static_cast<int>(line - begin()), index));

	// mark affected lines for colorization
	for (auto j = start.line; j <= end.line; j++) {
		at(j).colorize = true;
	}

	// update maximum column counts
	updateMaximumColumn(start.line, end.line);

	updated = true;
	return end;
}


//
//	TextEditor::Document::deleteText
//

void TextEditor::Document::deleteText(Coordinate start, Coordinate end) {
	auto& startLine = at(start.line);
	auto& endLine = at(end.line);
	auto startIndex = getIndex(start);
	auto endIndex = getIndex(end);

	// see if start and end are on the same line
	if (start.line == end.line) {
		startLine.erase(startLine.begin() + startIndex, startLine.begin() + endIndex);

	// start and end are on different lines
	} else {
		// remove end of first line
		startLine.erase(startLine.begin() + startIndex, startLine.end());

		// remove start of last line
		endLine.erase(endLine.begin(), endLine.begin() + endIndex);

		// join lines
		startLine.insert(startLine.end(), endLine.begin(), endLine.end());

		// delete lines
		deleteLines(start.line + 1, end.line);
	}

	// mark affected lines for colorization
	auto last = (start.line == lineCount() - 1) ? start.line : start.line + 1;

	for (auto line = start.line; line <= last; line++) {
		at(line).colorize = true;
	}

	// update maximum column counts
	updateMaximumColumn(start.line, end.line);
	updated = true;
}


//
//	TextEditor::Document::getText
//

std::string TextEditor::Document::getText() const {
	// process all glyphs and generate UTF-8 output
	std::string text;
	char utf8[4];

	for (auto line = begin(); line < end(); line++) {
		for (auto glyph = line->begin(); glyph < line->end(); glyph++) {
			text.append(std::string_view(utf8, CodePoint::write(utf8, glyph->codepoint)));
		}

		if (line < end() - 1) {
			text += "\n";
		}
	}

	return text;
}


//
//	TextEditor::Document::getLineText
//

std::string TextEditor::Document::getLineText(int line) const {
	return getSectionText(Coordinate(line, 0), Coordinate(line, at(line).maxColumn));
}


//
//	TextEditor::Document::getSectionText
//

std::string TextEditor::Document::getSectionText(Coordinate start, Coordinate end) const {
	std::string section;

	auto lineNo = start.line;
	auto index = getIndex(start);
	auto endIndex = getIndex(end);
	char utf8[4];

	while (lineNo < end.line || index < endIndex) {
		auto& line = at(lineNo);

		if (index < line.size()) {
			section.append(std::string_view(utf8, CodePoint::write(utf8, line[index].codepoint)));
			index++;

		} else {
			section += '\n';
			lineNo++;
			index = 0;
		}
	}

	return section;
}


//
//	TextEditor::Document::getCodePoint
//

ImWchar TextEditor::Document::getCodePoint(Coordinate location) {
	auto index = getIndex(location);

	if (index < at(location.line).size()) {
		return at(location.line)[index].codepoint;

	} else {
		return IM_UNICODE_CODEPOINT_INVALID;
	}
}


//
//	TextEditor::Document::updateMaximumColumn
//

void TextEditor::Document::updateMaximumColumn(int first, int last) {
	// process specified lines
	for (auto line = begin() + first; line <= begin() + last; line++) {
		// determine the maximum column number for this line
		int column = 0;

		for (auto glyph = line->begin(); glyph < line->end(); glyph++) {
			column = (glyph->codepoint == '\t') ? ((column / tabSize) + 1) * tabSize : column + 1;
		}

		line->maxColumn = column;
	}

	// determine maximum column number in document
	maxColumn = 0;

	for (auto line = begin(); line < end(); line++) {
		maxColumn = std::max(maxColumn, line->maxColumn);
	}
}


//
//	TextEditor::Document::getIndex
//

size_t TextEditor::Document::getIndex(const Line& line, int column) const {
	// convert a column reference to a glyph index for a specified line (taking tabs into account)
	auto end = line.end();
	size_t index = 0;
	auto leftCol = 0;
	auto rightCol = 0;

	for (auto glyph = line.begin(); rightCol < column && glyph < end; glyph++) {
		leftCol = rightCol;
		rightCol = (glyph->codepoint == '\t') ? ((rightCol / tabSize) + 1) * tabSize : rightCol + 1;
		index++;
	}

	if (rightCol - leftCol <= 1) {
		return index;

	} else {
		auto leftDiff = column - leftCol;
		auto rightDiff = rightCol - column;
		return leftDiff <= rightDiff ? index - 1 : index;
	}
}


//
//	TextEditor::Document::getColumn
//

int TextEditor::Document::getColumn(const Line& line, size_t index) const {
	// convert a glyph index to a column reference for the specified line (taking tabs into account)
	auto end = line.begin() + index;
	int column = 0;

	for (auto glyph = line.begin(); glyph < end; glyph++) {
		column = (glyph->codepoint == '\t') ? ((column / tabSize) + 1) * tabSize : column + 1;
	}

	return column;
}


//
//	TextEditor::Document::getUp
//

TextEditor::Coordinate TextEditor::Document::getUp(Coordinate from, int lines) const {
	return normalizeCoordinate(Coordinate(from.line - lines, from.column));
}


//
//	TextEditor::Document::getDown
//

TextEditor::Coordinate TextEditor::Document::getDown(Coordinate from, int lines) const {
	return normalizeCoordinate(Coordinate(from.line + lines, from.column));
}


//
//	TextEditor::Document::getLeft
//

TextEditor::Coordinate TextEditor::Document::getLeft(Coordinate from, bool wordMode) const {
	if (wordMode) {
		// first move left by one glyph
		from = getLeft(from);

		// now skip all whitespaces
		from = findPreviousNonWhiteSpace(from, false);

		// find the start of the current word
		return findWordStart(from);

	} else {
		// calculate coordinate of previous glyph (could be on previous line)
		auto index = getIndex(from);

		if (index == 0) {
			return (from.line > 0) ? Coordinate(from.line - 1, at(from.line - 1).maxColumn) : from;

		} else {
			return Coordinate(from.line, getColumn(from.line, index - 1));
		}
	}
}


//
//	TextEditor::Document::getRight
//

TextEditor::Coordinate TextEditor::Document::getRight(Coordinate from, bool wordMode) const {
	if (wordMode) {
		// first move right by one glyph
		from = getRight(from);

		// now skip all whitespaces
		from = findNextNonWhiteSpace(from, false);

		// find the end of the current word
		auto index = getIndex(from);
		return findWordEnd(Coordinate(from.line, getColumn(from.line, index)));

	} else {
		// calculate coordinate of next glyph (could be on next line)
		auto index = getIndex(from);

		if (index == at(from.line).size()) {
			return (from.line < lineCount() - 1) ? Coordinate(from.line + 1, 0) : from;

		} else {
			return Coordinate(from.line, getColumn(from.line, index + 1));
		}
	}
}


//
//	TextEditor::Document::getTop
//

TextEditor::Coordinate TextEditor::Document::getTop() const {
	return Coordinate(0, 0);
}


//
//	TextEditor::Document::getBottom
//

TextEditor::Coordinate TextEditor::Document::getBottom() const {
	auto lastLine = lineCount() - 1;
	return Coordinate(lastLine, at(lastLine).maxColumn);
}


//
//	TextEditor::Document::getStartOfLine
//

TextEditor::Coordinate TextEditor::Document::getStartOfLine(Coordinate from) const {
	return Coordinate(from.line, 0);
}


//
//	TextEditor::Document::getEndOfLine
//

TextEditor::Coordinate TextEditor::Document::getEndOfLine(Coordinate from) const {
	return Coordinate(from.line, at(from.line).maxColumn);
}


//
//	TextEditor::Document::findWordStart
//

TextEditor::Coordinate TextEditor::Document::findWordStart(Coordinate from, bool wordOnly) const {
	auto& line = at(from.line);
	auto lineSize = line.size();

	if (from.column == 0 || lineSize == 0) {
		return from;

	} else {
		auto index = getIndex(from);

		if (index == lineSize) {
			index--;
		}

		auto firstCharacter = line[index].codepoint;

		if (!wordOnly && CodePoint::isWhiteSpace(firstCharacter)) {
			while (index > 0 && CodePoint::isWhiteSpace(line[index - 1].codepoint)) {
				index--;
			}

		} else if (CodePoint::isWord(firstCharacter)) {
			while (index > 0 && CodePoint::isWord(line[index - 1].codepoint)) {
				index--;
			}

		} else {
			while (!wordOnly && index > 0 && !CodePoint::isWord(line[index - 1].codepoint) && !CodePoint::isWhiteSpace(line[index - 1].codepoint)) {
				index--;
			}
		}

		return Coordinate(from.line, getColumn(line, index));
	}
}


//
//	TextEditor::Document::findWordEnd
//

TextEditor::Coordinate TextEditor::Document::findWordEnd(Coordinate from, bool wordOnly) const {
	auto& line = at(from.line);
	auto index = getIndex(from);
	auto size = line.size();

	if (index >= size) {
		return from;

	} else {
		auto firstCharacter = line[index].codepoint;

		if (!wordOnly && CodePoint::isWhiteSpace(firstCharacter)) {
			while (index < size && CodePoint::isWhiteSpace(line[index].codepoint)) {
				index++;
			}

		} else if (CodePoint::isWord(firstCharacter)) {
			while (index < size && CodePoint::isWord(line[index].codepoint)) {
				index++;
			}

		} else {
			while (!wordOnly && index < size && !CodePoint::isWord(line[index].codepoint) && !CodePoint::isWhiteSpace(line[index].codepoint)) {
				index++;
			}
		}
	}

	return Coordinate(from.line, getColumn(line, index));
}


//
//	TextEditor::Document::findText
//

bool TextEditor::Document::findText(Coordinate from, const std::string_view& text, bool caseSensitive, bool wholeWord, Coordinate& start, Coordinate& end) const {
	// convert input string to vector of codepoints
	std::vector<ImWchar> search;
	auto endOfText = text.end();
	auto i = text.begin();

	while (i < endOfText) {
		ImWchar character;
		i = CodePoint::read(i, endOfText, &character);
		search.emplace_back(caseSensitive ? character : CodePoint::toLower(character));
	}

	// search document
	auto startLine = from.line;
	auto startIndex = getIndex(from);
	auto searchLine = startLine;
	auto searchIndex = startIndex;

	do {
		auto line = searchLine;
		auto index = searchIndex;
		auto lineSize = at(line).size();
		bool done = false;
		size_t j = 0;

		while (!done && j < search.size()) {
			if (search[j] == '\n') {
				if (index == lineSize) {
					if (line == lineCount() - 1) {
						done = true;

					} else {
						line++;
						index = 0;
						lineSize = at(line).size();
						j++;
					}

				} else {
					done = true;
				}

			} else {
				if (index == lineSize) {
					done = true;

				} else {
					auto ch = at(line)[index].codepoint;

					if (!caseSensitive) {
						ch = CodePoint::toLower(ch);
					}

					if (ch == search[j]) {
						index++;
						j++;

					} else {
						done = true;
					}
				}
			}
		}

		if (j == search.size()) {
			start = Coordinate(searchLine, getColumn(searchLine, searchIndex));
			end = Coordinate(line, getColumn(line, index));

			if (!wholeWord || isWholeWord(start, end)) {
				return true;
			}
		}

		if (searchIndex == at(searchLine).size()) {
			searchLine = (searchLine == lineCount() - 1) ? 0 : searchLine + 1;
			searchIndex = 0;

		} else {
			searchIndex++;
		}

	} while (searchLine != startLine || searchIndex != startIndex);

	return false;
}


//
//	TextEditor::Document::setUserData
//

void TextEditor::Document::setUserData(int line, void* data) {
	if (line >= 0 && line < lineCount()) {
		at(static_cast<size_t>(line)).userData = data;
	}
}


//
//	TextEditor::Document::getUserData
//

void* TextEditor::Document::getUserData(int line) const {
	if (line >= 0 && line < lineCount()) {
		return at(static_cast<size_t>(line)).userData;

	} else {
		return nullptr;
	}
}

//
//	TextEditor::Document::iterateUserData
//

void TextEditor::Document::iterateUserData(std::function<void(int line, void* data)> callback) const {
	for (size_t i = 0; i < size(); i++) {
		callback(static_cast<int>(i), at(i).userData);
	}
}


//
//	TextEditor::Document::isWholeWord
//

bool TextEditor::Document::isWholeWord(Coordinate start, Coordinate end) const {
	if (start.line != end.line || end.column - start.column < 1) {
		return false;

	} else {
		auto wordStart = findWordStart(Coordinate(start.line, start.column + 1));
		auto wordEnd = findWordEnd(Coordinate(end.line, end.column - 1));
		return start == wordStart && end == wordEnd;
	}
}


//
//	TextEditor::Document::findPreviousNonWhiteSpace
//

TextEditor::Coordinate TextEditor::Document::findPreviousNonWhiteSpace(Coordinate from, bool includeEndOfLine) const {
	bool done = false;

	while (!done) {
		auto& line = at(from.line);
		auto index = getIndex(from);

		while (!done && index > 0) {
			index--;

			if (!CodePoint::isWhiteSpace(line[index].codepoint)) {
				from.column = getColumn(line, index);
				done = true;
			}
		}

		if (!done) {
			if (from.line == 0 || !includeEndOfLine) {
				from.column = 0;
				done = true;

			} else {
				from.line--;
				from.column = at(from.line).maxColumn;
			}
		}
	}

	return from;
}


//
//	TextEditor::Document::findNextNonWhiteSpace
//

TextEditor::Coordinate TextEditor::Document::findNextNonWhiteSpace(Coordinate from, bool includeEndOfLine) const {
	bool done = false;

	while (!done) {
		auto& line = at(from.line);
		auto index = getIndex(from);

		while (!done && index < line.size()) {
			if (CodePoint::isWhiteSpace(line[index].codepoint)) {
				index++;

			} else {
				from.column = getColumn(line, index);
				done = true;
			}
		}

		if (!done) {
			if (from.line == lineCount() || !includeEndOfLine) {
				from.column = line.maxColumn;
				done = true;

			} else {
				from.line++;
				from.column = 0;
			}
		}
	}

	return from;
}


//
//	TextEditor::Document::normalizeCoordinate
//

TextEditor::Coordinate TextEditor::Document::normalizeCoordinate(Coordinate coordinate) const {
	if (coordinate.line < 0) {
		return Coordinate(0, 0);

	} else if (coordinate.line >= lineCount()) {
		return Coordinate(lineCount() - 1, at(size() - 1).maxColumn);

	} else if (coordinate.column < 0) {
		return Coordinate(coordinate.line, 0);

	} else if (coordinate.column > at(coordinate.line).maxColumn) {
		return Coordinate(coordinate.line, at(coordinate.line).maxColumn);

	} else {
		// determine column numbers left and right of provided coordinate
		auto& line = at(coordinate.line);
		auto end = line.end();
		auto leftCol = 0;
		auto rightCol = 0;

		for (auto glyph = line.begin(); rightCol < coordinate.column && glyph < end; glyph++) {
			leftCol = rightCol;
			rightCol = (glyph->codepoint == '\t') ? ((rightCol / tabSize) + 1) * tabSize : rightCol + 1;
		}

		auto leftDiff = coordinate.column - leftCol;
		auto rightDiff = rightCol - coordinate.column;
		return Coordinate(coordinate.line, leftDiff <= rightDiff ? leftCol : rightCol);
	}
}


//
//	TextEditor::Document::normalizeCoordinate
//

void TextEditor::Document::normalizeCoordinate(float line, float column, Coordinate& glyphCoordinate, Coordinate& cursorCoordinate) const {
	// normalize coordinates by clamping them to the document and line range
	// the returned glyphCoordinate addresses the glyph pointed to by the line and column parameters
	// the returned cursorCoordinate returns the closest cursor position (which can be at the start or the end of the glyph)
	if (line < 0.0f) {
		glyphCoordinate = Coordinate(0, 0);
		cursorCoordinate = glyphCoordinate;

	} else if (line >= static_cast<float>(lineCount())) {
		glyphCoordinate = Coordinate(lineCount() - 1, at(lineCount() - 1).maxColumn);;
		cursorCoordinate = glyphCoordinate;

	} else {
		auto lineNo = static_cast<int>(line);

		if (column < 0.0f) {
			glyphCoordinate = Coordinate(lineNo, 0);
			cursorCoordinate = glyphCoordinate;

		} else if (column >= static_cast<float>(at(lineNo).maxColumn)) {
			glyphCoordinate = Coordinate(lineNo, at(lineNo).maxColumn);
			cursorCoordinate = glyphCoordinate;

		} else {
			// determine column numbers left and right of provided coordinate
			auto leftCol = 0;
			auto rightCol = 0;
			auto end = at(lineNo).end();

			for (auto glyph = at(lineNo).begin(); rightCol < column && glyph < end; glyph++) {
				leftCol = rightCol;
				rightCol = (glyph->codepoint == '\t') ? ((rightCol / tabSize) + 1) * tabSize : rightCol + 1;
			}

			auto leftDiff = column - static_cast<float>(leftCol);
			auto rightDiff = static_cast<float>(rightCol) - column;

			glyphCoordinate = Coordinate(lineNo, leftCol);
			cursorCoordinate = Coordinate(lineNo, leftDiff <= rightDiff ? leftCol : rightCol);
		}
	}
}


//
//	TextEditor::Document::appendLine
//

void TextEditor::Document::appendLine() {
	auto& line = emplace_back();

	if (insertor) {
		line.userData = insertor(static_cast<int>(size() - 1));
	}
}


//
//	TextEditor::Document::insertLine
//

void TextEditor::Document::insertLine(int offsset) {
	auto line = insert(begin() + offsset, Line());

	if (insertor) {
		line->userData = insertor(offsset);
	}
}


//
//	TextEditor::Document::deleteLines
//

void TextEditor::Document::deleteLines(int start, int end) {
	if (deletor) {
		for (auto i = start; i <= end; i++) {
			deletor(i, at(i).userData);
		}
	}

	erase(begin() + start, begin() + end);
}


//
//	TextEditor::Document::clearDocument
//

void TextEditor::Document::clearDocument() {
	if (deletor) {
		for (auto i = 0; i <= lineCount(); i++) {
			deletor(i, at(i).userData);
		}
	}

	clear();
}
