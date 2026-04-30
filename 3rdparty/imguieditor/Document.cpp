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

void TextEditor::Document::setText(Config& config, const std::string_view& text) {
	// reset document
	clearDocument();
	appendLine();

	// process UTF-8 and generate lines of glyphs
	auto end = text.end();
	auto i = CodePoint::skipBOM(text.begin(), end);

	while (i < end) {
		ImWchar character;
		i = CodePoint::read(i, end, &character);

		if (character == '\n') {
			appendLine();

		} else if (config.insertSpacesOnTabs && character == '\t') {
			auto spaces = ((back().size() / config.tabSize) + 1) * config.tabSize - back().size();

			for (size_t s = 0; s < spaces; s++) {
				back().emplace_back(Glyph(' ', Color::text));
			}

		} else if (character != '\r') {
			back().emplace_back(Glyph(character, Color::text));
		}
	}

	// calculate line indents
	updateIndents(config, 0, size() - 1);
	updated = true;
}


//
//	TextEditor::Document::setText
//

void TextEditor::Document::setText(Config& config, const std::vector<std::string_view>& text) {
	// reset document
	clearDocument();

	if (text.size()) {
		// process input UTF-8 and generate lines of glyphs
		for (auto& line : text) {
			appendLine();
			auto i = line.begin();
			auto end = line.end();

			while (i < end) {
				ImWchar character;
				i = CodePoint::read(i, end, &character);

				if (config.insertSpacesOnTabs && character == '\t') {
					auto spaces = ((back().size() / config.tabSize) + 1) * config.tabSize - back().size();

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

	// calculate line indents
	updateIndents(config, 0, size() - 1);
	updated = true;
}


//
//	TextEditor::Document::insertText
//

TextEditor::DocPos TextEditor::Document::insertText(Config& config, DocPos start, const std::string_view& text) {
	auto line = begin() + start.line;
	auto index = start.index;
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

		} else if (config.insertSpacesOnTabs && character == '\t') {
			auto spaces = ((index / config.tabSize) + 1) * config.tabSize - index;

			for (size_t s = 0; s < spaces; s++) {
				line->insert(line->begin() + (index++), Glyph(' ', Color::text));
			}

		} else if (character != '\r') {
			// insert next glyph
			line->insert(line->begin() + (index++), Glyph(character, Color::text));
		}
	}

	// determine end of insert
	auto end = DocPos(lineNo, index);

	// mark affected lines as changed
	for (auto j = start.line; j <= end.line; j++) {
		at(j).needsColorizing = true;
		at(j).needsTypeSetting = true;
	}

	// calculate line indents
	updateIndents(config, start.line, end.line);
	updated = true;
	return end;
}


//
//	TextEditor::Document::deleteText
//

void TextEditor::Document::deleteText(Config& config, DocPos start, DocPos end) {
	auto& startLine = at(start.line);
	auto startIndex = start.index;
	auto& endLine = at(end.line);
	auto endIndex = end.index;

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

	// remove marker
	startLine.marker = 0;

	// mark line and document as changed
	at(start.line).needsColorizing = true;
	at(start.line).needsTypeSetting = true;

	// calculate line indents
	updateIndents(config, start.line, start.line);
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

std::string TextEditor::Document::getLineText(size_t line) const {
	return getSectionText(DocPos(line, 0), DocPos(line, at(line).size()));
}


//
//	TextEditor::Document::getSectionText
//

std::string TextEditor::Document::getSectionText(DocPos start, DocPos end) const {
	std::string section;

	auto lineNo = start.line;
	auto index = start.index;
	char utf8[4];

	while (lineNo < end.line || index < end.index) {
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

ImWchar TextEditor::Document::getCodePoint(DocPos location) const {
	if (location.index < at(location.line).size()) {
		return at(location.line)[location.index].codepoint;

	} else {
		return IM_UNICODE_CODEPOINT_INVALID;
	}
}

//
//	TextEditor::Document::getColor
//

TextEditor::Color TextEditor::Document::getColor(DocPos location)  const {
	if (location.index < at(location.line).size()) {
		return at(location.line)[location.index].color;

	} else {
		return Color::text;
	}
}


//
//	TextEditor::Document::getLeft
//

TextEditor::DocPos TextEditor::Document::getLeft(DocPos from, bool wordMode) const {
	if (wordMode) {
		// first move left by one glyph
		from = getLeft(from);

		// now skip all whitespaces
		from = findPreviousNonWhiteSpace(from, false);

		// find the start of the current word
		return findWordStart(from);

	} else {
		// calculate position of previous glyph (could be on previous line)
		if (from.index == 0) {
			return getLeftBeforeHiddenLines(from);

		} else {
			return DocPos(from.line, from.index - 1);
		}
	}
}


//
//	TextEditor::Document::getRight
//

TextEditor::DocPos TextEditor::Document::getRight(DocPos from, bool wordMode) const {
	if (wordMode) {
		// first move right by one glyph
		from = getRight(from);

		// now skip all whitespaces
		from = findNextNonWhiteSpace(from, false);

		// find the end of the current word
		return findWordEnd(from);

	} else {
		// calculate pos of next glyph (could be on next line)
		if (from.index == at(from.line).size()) {
			return getRightAfterHiddenLines(from);

		} else {
			return DocPos(from.line, from.index + 1);
		}
	}
}


//
//	TextEditor::Document::getTop
//

TextEditor::DocPos TextEditor::Document::getTop() const {
	return DocPos(0, 0);
}


//
//	TextEditor::Document::getBottom
//

TextEditor::DocPos TextEditor::Document::getBottom() const {
	auto lastLine = size() - 1;
	return DocPos(lastLine, at(lastLine).size());
}


//
//	TextEditor::Document::getStartOfLine
//

TextEditor::DocPos TextEditor::Document::getStartOfLine(DocPos from) const {
	return DocPos(from.line, 0);
}


//
//	TextEditor::Document::getEndOfLine
//

TextEditor::DocPos TextEditor::Document::getEndOfLine(DocPos from) const {
	return DocPos(from.line, at(from.line).size());
}


//
//	TextEditor::Document::findWordStart
//

TextEditor::DocPos TextEditor::Document::findWordStart(DocPos from, bool wordOnly) const {
	auto& line = at(from.line);
	auto lineSize = line.size();

	if (from.index == 0 || lineSize == 0) {
		return from;

	} else {
		auto index = from.index;
		auto firstCharacter = line[index - 1].codepoint;

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

		return DocPos(from.line, index);
	}
}


//
//	TextEditor::Document::findWordEnd
//

TextEditor::DocPos TextEditor::Document::findWordEnd(DocPos from, bool wordOnly) const {
	auto& line = at(from.line);
	auto index = from.index;
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

	return DocPos(from.line, index);
}


//
//	TextEditor::Document::findText
//

bool TextEditor::Document::findText(DocPos from, const std::string_view& text, bool caseSensitive, bool wholeWord, DocPos& start, DocPos& end) const {
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
	auto startIndex = from.index;
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
					if (line == size() - 1) {
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
			start = DocPos(searchLine, searchIndex);
			end = DocPos(line, index);

			if (!wholeWord || isWholeWord(start, end)) {
				return true;
			}
		}

		if (searchIndex == at(searchLine).size()) {
			searchLine = (searchLine == size() - 1) ? 0 : searchLine + 1;
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

void TextEditor::Document::setUserData(size_t line, void* data) {
	if (line < size()) {
		at(static_cast<size_t>(line)).userData = data;
	}
}


//
//	TextEditor::Document::getUserData
//

void* TextEditor::Document::getUserData(size_t line) const {
	if (line < size()) {
		return at(static_cast<size_t>(line)).userData;

	} else {
		return nullptr;
	}
}

//
//	TextEditor::Document::iterateUserData
//

void TextEditor::Document::iterateUserData(std::function<void(size_t line, void* data)> callback) const {
	for (size_t i = 0; i < size(); i++) {
		callback(i, at(i).userData);
	}
}


//
//	isIdentifier
//

static inline bool isIdentifier(TextEditor::Color color) {
	return
		color == TextEditor::Color::identifier ||
		color == TextEditor::Color::knownIdentifier;
}


//
//	TextEditor::Document::iterateIdentifiers
//

void TextEditor::Document::iterateIdentifiers(std::function<void(const std::string&)> callback) const {
	for (size_t i = 0; i < size(); i++) {
		auto p = at(i).begin();
		auto end = at(i).end();
		char utf8[4];

		while (p < end) {
			if (isIdentifier(p->color)) {
				std::string identifier;

				while (p < end && isIdentifier(p->color)) {
					identifier.append(std::string_view(utf8, CodePoint::write(utf8, p->codepoint)));
					p++;
				}

				callback(identifier);

			} else {
				p++;
			}
		}
	}
}


//
//	TextEditor::Document::isWholeWord
//

bool TextEditor::Document::isWholeWord(DocPos start, DocPos end) const {
	if (start.line != end.line || end.index - start.index < 1) {
		return false;

	} else {
		auto wordStart = findWordStart(DocPos(start.line, start.index + 1));
		auto wordEnd = findWordEnd(DocPos(end.line, end.index - 1));
		return start == wordStart && end == wordEnd;
	}
}


//
//	TextEditor::Document::findPreviousNonWhiteSpace
//

TextEditor::DocPos TextEditor::Document::findPreviousNonWhiteSpace(DocPos from, bool includeEndOfLine) const {
	bool done = false;

	while (!done) {
		auto& line = at(from.line);
		auto index = from.index;

		while (!done && index > 0) {
			index--;

			if (!CodePoint::isWhiteSpace(line[index].codepoint)) {
				from.index = index;
				done = true;
			}
		}

		if (!done) {
			if (from.line == 0 || !includeEndOfLine) {
				from.index = 0;
				done = true;

			} else {
				from = getLeftBeforeHiddenLines(from);
			}
		}
	}

	return from;
}


//
//	TextEditor::Document::findNextNonWhiteSpace
//

TextEditor::DocPos TextEditor::Document::findNextNonWhiteSpace(DocPos from, bool includeEndOfLine) const {
	bool done = false;

	while (!done) {
		auto& line = at(from.line);
		auto index = from.index;

		while (!done && index < line.size()) {
			if (CodePoint::isWhiteSpace(line[index].codepoint)) {
				index++;

			} else {
				from.index = index;
				done = true;
			}
		}

		if (!done) {
			if (from.line == size() || !includeEndOfLine) {
				from.index = line.size();
				done = true;

			} else {
				from = getRightAfterHiddenLines(from);
			}
		}
	}

	return from;
}


//
//	TextEditor::Document::normalizePos
//

TextEditor::DocPos TextEditor::Document::normalizePos(DocPos pos) const {
	if (pos.line >= size()) {
		return getBottom();

	} else if (pos.index > at(pos.line).size()) {
		return getEndOfLine(pos);

	} else {
		return pos;
	}
}


//
//	TextEditor::Document::getLeftBeforeHiddenLines
//

TextEditor::DocPos TextEditor::Document::getLeftBeforeHiddenLines(DocPos pos) const {
	if (pos.line == 0) {
		return pos;

	} else {
		auto line = pos.line - 1;

		while (line > 0 && at(line).foldingState == FoldingState::hidden) {
			line--;
		}

		return DocPos(line, at(line).size());
	}
}


//
//	TextEditor::Document::getRightAfterHiddenLines
//

TextEditor::DocPos TextEditor::Document::getRightAfterHiddenLines(DocPos pos) const {
	if (pos.line == size() - 1) {
		return pos;

	} else {
		auto line = pos.line + 1;

		while (line < size() - 1 && at(line).foldingState == FoldingState::hidden) {
			line++;
		}

		return DocPos(line, 0);
	}
}


//
//	TextEditor::Document::appendLine
//

void TextEditor::Document::appendLine() {
	auto& line = emplace_back();

	if (insertor) {
		line.userData = insertor(size() - 1);
	}
}


//
//	TextEditor::Document::insertLine
//

void TextEditor::Document::insertLine(size_t offset) {
	auto line = insert(begin() + offset, Line());

	if (insertor) {
		line->userData = insertor(offset);
	}
}


//
//	TextEditor::Document::deleteLines
//

void TextEditor::Document::deleteLines(size_t start, size_t end) {
	if (deletor) {
		for (auto i = start; i <= end; i++) {
			deletor(i, at(i).userData);
		}
	}

	erase(begin() + start, begin() + end + 1);
}


//
//	TextEditor::Document::clearDocument
//

void TextEditor::Document::clearDocument() {
	if (deletor) {
		for (size_t i = 0; i <= size(); i++) {
			deletor(i, at(i).userData);
		}
	}

	clear();
}


//
//	TextEditor::Document::updateIndents
//

void TextEditor::Document::updateIndents(Config& config, size_t start, size_t end) {
	for (size_t i = start; i <= end; i++) {
		auto& line = at(i);
		line.indent = 0;
		bool done = false;

		for (size_t j = 0; j < line.size() && !done; j++) {
			if (line[j].codepoint == ' ') {
				line.indent++;

			} else if (line[j].codepoint == '\t') {
				line.indent += config.tabSize - (line.indent % config.tabSize);

			} else {
				done = true;
			}
		}
	}
}
