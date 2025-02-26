//	TextEditor - A syntax highlighting text editor for Dear ImGui.
//	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "TextEditor.h"


//
//	TextEditor::Bracketeer::reset
//

void TextEditor::Bracketeer::reset() {
	clear();
}


//
//	TextEditor::Bracketeer::update
//

void TextEditor::Bracketeer::update(Document& document) {
	Color bracketColors[] = {
		Color::matchingBracketLevel1,
		Color::matchingBracketLevel2,
		Color::matchingBracketLevel3
	};

	reset();
	std::vector<size_t> levels;
	int level = 0;

	// process all the glyphs
	for (int line = 0; line < document.lineCount(); line++) {
		for (size_t index = 0; index < document[line].size(); index++) {
			auto& glyph = document[line][index];

			// handle a "bracket opener" that is not in a comment, string or preprocessor statement
			if (isBracketCandidate(glyph) && isBracketOpener(glyph.codepoint)) {
				// start a new level
				levels.emplace_back(size());
				emplace_back(glyph.codepoint, Coordinate(line, document.getColumn(line, index)), static_cast<ImWchar>(0), Coordinate::invalid(), level);
				glyph.color = bracketColors[level % 3];
				level++;

			// handle a "bracket closer" that is not in a comment, string or preprocessor statement
			} else if (isBracketCandidate(glyph) && isBracketCloser(glyph.codepoint)) {
				if (levels.size()) {
					auto& lastBracket = at(levels.back());
					levels.pop_back();
					level--;

					if (lastBracket.startChar == toBracketOpener(glyph.codepoint)) {
						// handle matching bracket
						glyph.color = bracketColors[level % 3];
						lastBracket.endChar = glyph.codepoint;
						lastBracket.end = Coordinate(line, document.getColumn(line, index));

					} else {
						// no matching bracket, mark brackets as errors
						glyph.color = Color::matchingBracketError;
						document[lastBracket.start.line][document.getIndex(lastBracket.start)].color = Color::matchingBracketError;
						pop_back();
					}

				// this is a closer without an opener
				} else {
					glyph.color = Color::matchingBracketError;
				}
			}
		}
	}

	// handle levels left open and mark them as errors
	if (levels.size()) {
		for (auto i = levels.rbegin(); i < levels.rend(); i++) {
			auto& start = at(*i).start;
			document[start.line][document.getIndex(start)].color = Color::matchingBracketError;
			erase(begin() + *i);
		}
	}
}


//
//	TextEditor::Bracketeer::getEnclosingCurlyBrackets
//

TextEditor::Bracketeer::iterator TextEditor::Bracketeer::getEnclosingBrackets(Coordinate location) {
	iterator brackets = end();
	bool done = false;

	for (auto i = begin(); !done && i < end(); i++) {
		// brackets are sorted so no need to go past specified location
		if (i->isAfter(location)) {
			done = true;
		}

		else if (i->isAround(location)) {
			// this could be what we're looking for
			brackets = i;
		}
	}

	return brackets;
}


//
//	TextEditor::Bracketeer::getEnclosingCurlyBrackets
//

TextEditor::Bracketeer::iterator TextEditor::Bracketeer::getEnclosingCurlyBrackets(Coordinate location) {
	iterator brackets = end();
	bool done = false;

	for (auto i = begin(); !done && i < end(); i++) {
		// brackets are sorted so no need to go past specified location
		if (i->isAfter(location)) {
			done = true;
		}

		else if (i->isAround(location) && i->startChar == '{') {
			// this could be what we're looking for
			brackets = i;
		}
	}

	return brackets;
}


//
//	TextEditor::Bracketeer::getInnerCurlyBrackets
//

TextEditor::Bracketeer::iterator TextEditor::Bracketeer::getInnerCurlyBrackets(Coordinate location) {
	auto brackets = getEnclosingCurlyBrackets(location);

	if (brackets != end()) {
		bool done = false;

		for (auto next = brackets + 1; next < end() && !done; next++) {
			 if (next->level == brackets->level + 1 && next->startChar == '{') {
				brackets = next;
				done = true;

			} else if (next->level <= brackets->level) {
				done = true;
			 }
		}
	}

	return brackets;
}
