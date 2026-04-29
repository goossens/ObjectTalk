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
//	TextEditor::Bracketeer::update
//

void TextEditor::Bracketeer::update(Config& config, Document& document) {
	// see if the configuration changed
	bool configChanged =
		showMatchingBrackets != config.showMatchingBrackets ||
		language != config.language;

	showMatchingBrackets = config.showMatchingBrackets;
	language = config.language;

	if (configChanged && !showMatchingBrackets) {
		// if configuration changed to not showing matching brackets, just clear the bracket pair list
		clear();

	} else if (configChanged || document.isUpdated()) {
		// if configuration or document changed, recalculate bracket pair list
		static const Color bracketColors[] = {
			Color::matchingBracketLevel1,
			Color::matchingBracketLevel2,
			Color::matchingBracketLevel3
		};

		// copy old list so we can see if things have changed
		auto previous = *this;

		// clear old list
		clear();
		std::vector<size_t> levels;
		size_t level = 0;

		// process all the glyphs
		for (size_t line = 0; line < document.size(); line++) {
			for (size_t index = 0; index < document[line].size(); index++) {
				auto& glyph = document[line][index];

				// handle a "bracket opener" that is not in a comment, string or preprocessor statement
				if (isBracketCandidate(glyph) && CodePoint::isBracketOpener(glyph.codepoint)) {
					// start a new level
					levels.emplace_back(size());
					emplace_back(glyph.codepoint, DocPos(line, index), static_cast<ImWchar>(0), DocPos(0, 0), level, true);
					glyph.color = bracketColors[level % 3];
					level++;

				// handle a "bracket closer" that is not in a comment, string or preprocessor statement
				} else if (isBracketCandidate(glyph) && CodePoint::isBracketCloser(glyph.codepoint)) {
					if (levels.size()) {
						auto& lastBracket = at(levels.back());
						levels.pop_back();
						level--;

						if (lastBracket.startChar == CodePoint::toPairOpener(glyph.codepoint)) {
							// handle matching bracket
							glyph.color = bracketColors[level % 3];
							lastBracket.endChar = glyph.codepoint;
							lastBracket.end = DocPos(line, index);

						} else {
							// no matching bracket, mark brackets as errors
							glyph.color = Color::matchingBracketError;
							document[lastBracket.start.line][lastBracket.start.index].color = Color::matchingBracketError;
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
				document[start.line][start.index].color = Color::matchingBracketError;
				erase(begin() + *i);
			}
		}

		// process invisible blocks for languages that are indentation based
		if (language && language->indentationForBlocks) {
			for (size_t i = 0; i < document.size(); i++) {
				if (document[i].size()) {
					auto currentIndent = document[i].indent;
					auto endLine = i;
					auto done = false;

					for (size_t j = i + 1; j < document.size() && !done; j++) {
						if (document[j].size()) {
							auto nextIndent = document[j].indent;

							if (nextIndent > currentIndent) {
								endLine = j;

							} else {
								done = true;
							}
						}
					}

					if (endLine > i) {
						emplace_back(DocPos(i, 0), DocPos(endLine, document[endLine].size()));
					}
				}
			}

			// sort visible and invisible blocks by block start
			std::sort(begin(), end(), [](BracketPair& a, BracketPair& b) {
				return a.start < b.start;
			});
		}

		updated = previous != *this;
	}
}


//
//	TextEditor::Bracketeer::getEnclosingBrackets
//

TextEditor::Bracketeer::iterator TextEditor::Bracketeer::getEnclosingBrackets(DocPos location) {
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
//	TextEditor::Bracketeer::getEnclosingBrackets
//

TextEditor::Bracketeer::iterator TextEditor::Bracketeer::getEnclosingBrackets(DocPos first, DocPos last) {
	iterator brackets = end();
	bool done = false;

	for (auto i = begin(); !done && i < end(); i++) {
		// brackets are sorted so no need to go past specified location
		if (i->isAfter(first)) {
			done = true;
		}

		else if (i->isAround(first) && i->isAround(last)) {
			// this could be what we're looking for
			brackets = i;
		}
	}

	return brackets;
}


//
//	TextEditor::Bracketeer::getInnerBrackets
//

TextEditor::Bracketeer::iterator TextEditor::Bracketeer::getInnerBrackets(DocPos first, DocPos last) {
	iterator brackets = end();
	auto outer = getEnclosingBrackets(first, last);

	if (outer != end()) {
		bool done = false;

		for (auto i = outer + 1; i < end() && !done; i++) {
			if (i->level <= outer->level) {
				done = true;

			} else if (i->level == outer->level + 1 && i->start > first && i->end < last) {
				brackets = i;
				done = true;
			}
		}
	}

	return brackets;
}
