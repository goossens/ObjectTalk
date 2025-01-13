//	TextEditor - A syntax highlighting text editor for ImGui
//	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "TextEditor.h"


//
//	TextEditor::Line::maxColumn
//

int TextEditor::Line::maxColumn(int tabSize) const {
	// determine the maximum column number for this line
	int column = 0;

	for (auto glyph = begin(); glyph < end(); glyph++) {
		column = (glyph->character == '\t') ? ((column / tabSize) + 1) * tabSize : column + 1;
	}

	return column;
}


//
//	TextEditor::Line::columnToIndex
//

int TextEditor::Line::columnToIndex(int column, int tabSize) const {
	// convert a column reference to a glyph index for this line (taking tabs into account)
	int index = 0;
	int c = 0;

	for (auto glyph = begin(); glyph < end(); glyph++) {
		if (c == column) {
			return index;

		} if (c > column) {
			return index - 1;
		}

		c = (glyph->character == '\t') ? ((c / tabSize) + 1) * tabSize : c + 1;
		index++;
	}

	return index;
}


//
//	TextEditor::Line::indexToColumn
//

int TextEditor::Line::indexToColumn(int index, int tabSize) const {
	// convert a glyph index to a column reference for this line (taking tabs into account)
	int column = 0;

	for (auto glyph = begin(); glyph < begin() + index; glyph++) {
		column = (glyph->character == '\t') ? ((column / tabSize) + 1) * tabSize : column + 1;
	}

	return column;
}


//
//	TextEditor::Document::setText
//

void TextEditor::Document::setText(const std::string& text) {
	// reset document
	clear();
	emplace_back();

	// process input UTF-8 and generate lines of glyphs
	auto i = text.begin();

	while (i < text.end()) {
		ImWchar character;
		i = CodePoint::get(i, &character);

		if (character == '\n') {
			emplace_back();

		} else if (character != '\r') {
			back().emplace_back(Glyph(character, Color::text));
		}
	}

	// colorize the text
	colorize();
	updated = true;
}


//
//	TextEditor::Document::insertText
//

TextEditor::Coordinate TextEditor::Document::insertText(Coordinate start, const std::string& text) {
	auto line = begin() + start.line;
	auto index = getIndex(start);
	auto lineNo = start.line;

	// process input as UTF-8
	auto i = text.begin();

	// process all codepoints
	while (i < text.end()) {
		ImWchar character;
		i = CodePoint::get(i, &character);

		if (character == '\n') {
			// split line
			insert(line + 1, Line());
			line = begin() + lineNo;
			auto nextLine = begin() + ++lineNo;

			for (auto i = line->begin() + index; i < line->end(); i++) {
				nextLine->push_back(*i);
			}

			line->erase(line->begin() + index, line->end());
			line = nextLine;
			index = 0;

		} else if (character != '\r') {
			// insert next glyph
			line->insert(line->begin() + (index++), Glyph(character, Color::text));
		}
	}

	// determine end-of insert
	auto end = Coordinate(lineNo, getColumn(static_cast<int>(line - begin()), index));

	// colorize the new lines
	colorize(start.line, end.line);
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
		// remove start of last line
		endLine.erase(endLine.begin(), endLine.begin() + endIndex);

		// remove full lines
		erase(begin() + start.line + 1, begin() + end.line);

		// remove end of first line
		auto& startLine = at(start.line);
		startLine.erase(startLine.begin() + startIndex, startLine.end());

		// join lines
		auto& nextLine = at(start.line + 1);
		startLine.insert(startLine.end(), nextLine.begin(), nextLine.end());
		erase(begin() + start.line + 1);
	}

	// colorize around the deleted line(s)
	colorize(start.line, (start.line == lines() - 1) ? start.line : start.line + 1);
	updated = true;
}


//
//	TextEditor::Document::getText
//

std::string TextEditor::Document::getText() const {
	// process all glyphs and generate UTF-8 output
	std::string text;
	std::string utf8(4, 0);

	for (auto i = begin(); i < end(); i++) {
		for (auto& glyph : *i) {
			text.append(utf8.begin(), CodePoint::put(utf8.begin(), glyph.character));
		}

		if (i < end() - 1) {
			text += "\n";
		}
	}

	return text;
}


//
//	TextEditor::Document::getSectionText
//

std::string TextEditor::Document::getSectionText(Coordinate start, Coordinate end) const {
	std::string section;

	auto lineNo = start.line;
	auto index = getIndex(start);
	auto endIndex = getIndex(end);
	std::string utf8(4, 0);

	while (lineNo < end.line || index < endIndex) {
		auto& line = at(lineNo);

		if (index < line.glyphs()) {
			section.append(utf8.begin(), CodePoint::put(utf8.begin(), line[index].character));

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
//	TextEditor::Document::getLineText
//

std::string TextEditor::Document::getLineText(int line) const {
	return getSectionText(Coordinate(line, 0), Coordinate(line, maxColumn(line)));
}


//
//	TextEditor::Document::maxColumn
//

int TextEditor::Document::maxColumn() const {
	// determine the maximum column number for this document
	int result = 0;

	for (auto& line : *this) {
		result = std::max(result, maxColumn(line));
	}

	return result;
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
		if (from.column == 0) {
			if (from.line) {
				from = Coordinate(from.line - 1, maxColumn(from.line - 1));
			}
		}

		return findWordStart(from);

	} else {
		auto index = getIndex(from);

		if (index == 0) {
			return (from.line > 0) ? Coordinate(from.line - 1, maxColumn(from.line - 1)) : from;

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
		if (isEndOfLine(from)) {
			if (from.line < lines() - 1) {
				from = Coordinate(from.line + 1, 0);
			}
		}

		return findWordEnd(from);

	} else {
		auto index = getIndex(from);

		if (index == at(from.line).glyphs()) {
			return (from.line < lines() - 1) ? Coordinate(from.line + 1, 0) : from;

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
	auto lastLine = lines() - 1;
	return Coordinate(lastLine, maxColumn(lastLine));
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
	return Coordinate(from.line, maxColumn(from.line));
}


//
//	TextEditor::Document::findWordStart
//

TextEditor::Coordinate TextEditor::Document::findWordStart(Coordinate from) const {
	if (from.column == 0) {
		return from;

	} else {
		auto& line = at(from.line);
		auto index = getIndex(from);
		auto firstCharacter = line[index - 1].character;

		if (CodePoint::isSpace(firstCharacter)) {
			while (index > 0 && CodePoint::isSpace(line[index - 1].character)) {
				index--;
			}

		} else if (language->isWord(firstCharacter)) {
			while (index > 0 && language->isWord(line[index - 1].character)) {
				index--;
			}

		} else if (language->isPunctuation(firstCharacter)) {
			while (index > 0 && language->isPunctuation(line[index - 1].character)) {
				index--;
			}

		} else if (index > 0) {
			index--;
		}

		return Coordinate(from.line, getColumn(line, index));
	}
}


//
//	TextEditor::Document::findWordEnd
//

TextEditor::Coordinate TextEditor::Document::findWordEnd(Coordinate from) const {
	auto& line = at(from.line);
	auto index = getIndex(from);
	auto size = line.glyphs();

	auto firstCharacter = line[index].character;

	if (CodePoint::isSpace(firstCharacter)) {
		while (index < size && CodePoint::isSpace(line[index].character)) {
			index++;
		}

	} else if (language->isWord(firstCharacter)) {
		while (index < size && language->isWord(line[index].character)) {
			index++;
		}

	} else if (language->isPunctuation(firstCharacter)) {
		while (index < size && language->isPunctuation(line[index].character)) {
			index++;
		}

	} else if (index < size) {
		index++;
	}

	return Coordinate(from.line, getColumn(line, index));
}


//
//	TextEditor::Document::findText
//

bool TextEditor::Document::findText(Coordinate from, const std::string& text, bool caseSensitive, bool wholeWord, Coordinate& start, Coordinate& end) const {
	// convert input string to vector of codepoints
	std::vector<ImWchar> search;
	auto i = text.begin();

	while (i < text.end()) {
		ImWchar character;
		i = CodePoint::get(i, &character);
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
		auto lineSize = at(line).glyphs();
		bool done = false;
		int i = 0;

		while (!done && i < search.size()) {
			if (search[i] == '\n') {
				if (index == lineSize) {
					if (line == lines() - 1) {
						done = true;

					} else {
						line++;
						index = 0;
						lineSize = at(line).glyphs();
						i++;
					}

				} else {
					done = true;
				}

			} else {
				if (index == lineSize) {
					done = true;

				} else {
					auto ch = at(line)[index].character;

					if (!caseSensitive) {
						ch = CodePoint::toLower(ch);
					}

					if (ch == search[i]) {
						index++;
						i++;

					} else {
						done = true;
					}
				}
			}
		}

		if (i == search.size()) {
			start = Coordinate(searchLine, getColumn(searchLine, searchIndex));
			end = Coordinate(line, getColumn(line, index));

			if (!wholeWord || isWholeWord(start, end)) {
				return true;
			}
		}

		if (searchIndex == at(searchLine).glyphs()) {
			searchLine = (searchLine == lines() - 1) ? 0 : searchLine + 1;
			searchIndex = 0;

		} else {
			searchIndex++;
		}

	} while (searchLine != startLine || searchIndex != startIndex);

	return false;
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
//	TextEditor::Document::normalizeCoordinate
//

TextEditor::Coordinate TextEditor::Document::normalizeCoordinate(Coordinate coordinate) const {
	Coordinate result;

	if (coordinate.line < 0) {
		result = Coordinate(0, 0);

	} else if (coordinate.line >= lines()) {
		result = Coordinate(lines() - 1, maxColumn(lines() - 1));

	} else {
		result = Coordinate(coordinate.line, std::max(0, std::min(coordinate.column, maxColumn(coordinate.line))));
	}

	return Coordinate(result.line, getColumn(result.line, getIndex(result)));
}

//
//	TextEditor::Document::setLanguage
//

void TextEditor::Document::setLanguage(const Language &definition) {
	language = &definition;
	colorize();
	updated = true;
}


//
//	TextEditor::Document::colorize
//

TextEditor::State TextEditor::Document::colorize(Line& line) {
	auto state = line.state;

	// process all glyphs in this line
	auto glyph = line.begin();
	auto nonWhiteSpace = false;

	while (glyph < line.end()) {
		if (state == State::inText) {
			// special handling for preprocessor lines
			if (language->preprocess && glyph->character != language->preprocess && !CodePoint::isSpace(glyph->character)) {
				nonWhiteSpace = true;
			}

			// mark whitespace characters
			if (CodePoint::isSpace(glyph->character)) {
				(glyph++)->color = Color::whitespace;

			// handle single line comments
			} else if (language->singleLineComment.size() && line.matches(glyph, language->singleLineComment)) {
				line.setColor(glyph, line.end(), Color::comment);
				glyph = line.end();

			} else if (language->singleLineCommentAlt.size() && line.matches(glyph, language->singleLineCommentAlt)) {
				line.setColor(glyph, line.end(), Color::comment);
				glyph = line.end();

			// are we starting a multiline comment
			} else if (language->commentStart.size() && line.matches(glyph, language->commentStart)) {
				state = State::inComment;
				auto size = language->commentEnd.size();
				line.setColor(glyph, glyph + size, Color::comment);
				glyph += size;

			// are we starting a special string
			} else if (language->otherStringStart.size() && line.matches(glyph, language->otherStringStart)) {
				state = State::inOtherString;
				auto size = language->otherStringStart.size();
				line.setColor(glyph, glyph + size, Color::string);
				glyph += size;

			} else if (language->otherStringAltStart.size() && line.matches(glyph, language->otherStringAltStart)) {
				state = State::inOtherStringAlt;
				auto size = language->otherStringAltStart.size();
				line.setColor(glyph, glyph + size, Color::string);
				glyph += size;

			// are we starting a single quoted string
			} else if (language->hasSingleQuotedStrings && glyph->character == '\'') {
				state = State::inSingleQuotedString;
				(glyph++)->color = Color::string;

			// are we starting a double quoted string
			} else if (language->hasDoubleQuotedStrings && glyph->character == '"') {
				state = State::inDoubleQuotedString;
				(glyph++)->color = Color::string;

			// is this a preprocessor line
			} else if (language->preprocess && !nonWhiteSpace && glyph->character == language->preprocess) {
				line.setColor(line.begin(), line.end(), Color::preprocessor);
				glyph = line.end();

			// nothing worked so far so it's time to do some tokenizing
			} else {
				Iterator start(static_cast<void*>(&line), static_cast<int>(glyph - line.begin()));
				Iterator end(static_cast<void*>(&line), line.glyphs());
				Iterator newEnd;

				// do we have an identifier
				if (language->getIdentifier && (newEnd = language->getIdentifier(start, end)) != start) {
					int size = newEnd - start;

					// determine identifier text and color color
					std::string identifier;
					auto color = Color::identifier;

					for (auto i = start; i < newEnd; i++) {
						identifier += *i;
					}

					if (language->keywords.find(identifier) != language->keywords.end()) {
						color = Color::keyword;

					} else if (language->declarations.find(identifier) != language->declarations.end()) {
						color = Color::declaration;

					} else if (language->identifiers.find(identifier) != language->identifiers.end()) {
						color = Color::knownIdentifier;
					}

					// colorize identifier and move on
					line.setColor(glyph, glyph + size, color);
					glyph += size;

				// do we have a number
				} else if (language->getNumber && (newEnd = language->getNumber(start, end)) != start) {
					int size = newEnd - start;
					line.setColor(glyph, glyph + size, Color::number);
					glyph += size;

				// is this punctuation
				} else if (language->isPunctuation(glyph->character)) {
					(glyph++)->color = Color::punctuation;

				} else {
					// I guess we didn't know what this character is
					(glyph++)->color = Color::text;
				}
			}

		} else if (state == State::inComment) {
			// stay in comment state until we see the end sequence
			if (line.matches(glyph, language->commentEnd)) {
				auto size = language->commentEnd.size();
				line.setColor(glyph, glyph + size, Color::comment);
				glyph += size;
				state = State::inText;

			} else {
				(glyph++)->color = Color::comment;
			}

		} else if (state == State::inOtherString) {
			// stay in otherString state until we see the end sequence
			// skip escaped character
			if (glyph->character == language->stringEscape) {
				(glyph++)->color = Color::string;

				if (glyph < line.end()) {
					(glyph++)->color = Color::string;
				}

			} else if (line.matches(glyph, language->otherStringEnd)) {
				auto size = language->otherStringEnd.size();
				line.setColor(glyph, glyph + size, Color::string);
				glyph += size;
				state = State::inText;

			} else {
				(glyph++)->color = Color::comment;
			}

		} else if (state == State::inOtherStringAlt) {
			// stay in otherStringAlt state until we see the end sequence
			// skip escaped character
			if (glyph->character == language->stringEscape) {
				(glyph++)->color = Color::string;

				if (glyph < line.end()) {
					(glyph++)->color = Color::string;
				}

			} else if (line.matches(glyph, language->otherStringAltEnd)) {
				auto size = language->otherStringAltEnd.size();
				line.setColor(glyph, glyph + size, Color::string);
				glyph += size;
				state = State::inText;

			} else {
				(glyph++)->color = Color::comment;
			}

		} else if (state == State::inSingleQuotedString) {
			// stay in single quote state until we see an end
			// skip escaped character
			if (glyph->character == language->stringEscape) {
				(glyph++)->color = Color::string;

				if (glyph < line.end()) {
					(glyph++)->color = Color::string;
				}

			} else if (glyph->character == '\'') {
				(glyph++)->color = Color::string;
				state = State::inText;

			} else {
				(glyph++)->color = Color::string;
			}

		} else if (state == State::inDoubleQuotedString) {
			// stay in double quote state until we see an end
			if (glyph->character == language->stringEscape) {
				// skip escaped character
				(glyph++)->color = Color::string;

				if (glyph < line.end()) {
					(glyph++)->color = Color::string;
				}

			} else if (glyph->character == '"') {
				(glyph++)->color = Color::string;
				state = State::inText;

			} else {
				(glyph++)->color = Color::string;
			}
		}
	}

	return state;
}

void TextEditor::Document::colorize(int start, int end) {
	if (language) {
		for (auto line = start; line <= end; line++) {
			State state = colorize(at(line));

			if (line < lines() - 1) {
				auto& nextLine = at(line + 1);

				if (nextLine.state != state) {
					nextLine.state = state;

					if (line == end) {
						end++;
					}
				}
			}
		}
	}
}

void TextEditor::Document::colorize() {
	if (language) {
		colorize(0, lines() - 1);
	}
}
