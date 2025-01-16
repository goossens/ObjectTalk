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
//	TextEditor::Brackets::update
//

void TextEditor::Brackets::update(Document& document) {
	Color bracketColors[] = {
		Color::matchingBracketLevel1,
		Color::matchingBracketLevel2,
		Color::matchingBracketLevel3
	};

	clear();
	std::vector<size_t> levels;
	int level = 0;

	// process all the glyphs
	for (auto line = 0; line < document.lines(); line++) {
		for (auto index = 0; index < document[line].glyphs(); index++) {
			auto& glyph = document[line][index];

			// handle a bracket opener that is not in a comment, string or preprocessor statement
			if (isBracketCandidate(glyph) && isBracketOpener(glyph.character)) {
				// start a new level
				levels.emplace_back(size());
				emplace_back(glyph.character, Coordinate(line, document.getColumn(line, index)), 0, Coordinate::invalid(), level);
				glyph.color = bracketColors[level % 3];
				level++;

			// handle a bracket closer that is not in a comment, string or preprocessor statement
			} else if (isBracketCandidate(glyph) && isBracketCloser(glyph.character)) {
				if (levels.size()) {
					auto& lastBracket = at(levels.back());
					levels.pop_back();
					level--;

					if (lastBracket.startChar == toBracketOpener(glyph.character)) {
						// handle matching bracket
						glyph.color = bracketColors[level % 3];
						lastBracket.endChar = glyph.character;
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
//	TextEditor::Brackets::getActive
//

TextEditor::Brackets::iterator TextEditor::Brackets::getActive(Coordinate location) {
	if (location != activeLocation) {
		active = end();
		bool done = false;

		for (auto i = begin(); i < end(); i++) {
			// skip pairs that start after specified location
			if (i->isAfter(location)) {
				done = true;
			}

			// brackets are active when they are around specified location
			else if (i->isAround(location)) {
				active = i;
			}
		}

		activeLocation = location;
	}

	return active;
}
