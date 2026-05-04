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
//	TextEditor::LineFold::update
//

bool TextEditor::LineFold::update(Config& config, Document& document, const Bracketeer& bracketeer) {
	// force update after previous (un)fold request
	if (forceUpdate) {
		forceUpdate = false;
		updated = true;
	}

	// see if configuration changed
	if (lineFolding != config.lineFolding) {
		lineFolding = config.lineFolding;

		if (lineFolding) {
			// line folding is turned on, ensure bracketeer is also active
			config.showMatchingBrackets = true;

		} else {
			// line folding is turned off, clear list and reset all lines
			clear();

			for (auto& line : document) {
				line.foldingState = FoldingState::visible;
			}
		}

		updated = true;
	}

	// see if bracket layout changed
	if (bracketeer.isUpdated()) {
		updated = true;
	}

	// (re)build list of folding opportunities (if feature is on and updates have occurred)
	if (lineFolding && updated) {
		std::unordered_set<size_t> previouslyFolded;
		clear();

		for (auto& bracketPair : bracketeer) {
			// brackets must at least be one line apart
			if (bracketPair.end.line - bracketPair.start.line >= 1) {
				// we preserve old folding state
				if (document[bracketPair.start.line].foldingState == FoldingState::folded) {
					previouslyFolded.insert(bracketPair.start.line);
				}

				// add to list
				emplace_back(bracketPair.start.line, bracketPair.end.line);
			}
		}

		// apply new folding state to lines in document
		for (auto& line : document) {
			line.foldingState = FoldingState::visible;
		}

		for (auto& fold : *this) {
			if (previouslyFolded.find(fold.start) != previouslyFolded.end()) {
				document[fold.start].foldingState = FoldingState::folded;

				for (size_t i = fold.start + 1; i <= fold.end; i++) {
					document[i].foldingState = FoldingState::hidden;
				}

			} else if (document[fold.start].foldingState != FoldingState::hidden) {
				document[fold.start].foldingState = FoldingState::foldable;
			}
		}
	}

	return updated;
}


//
//	TextEditor::LineFold::foldAroundLine
//

void TextEditor::LineFold::foldAroundLine(Document& document, size_t line) {
	auto lineToFold = invalidLine;

	for (auto& fold : *this) {
		if (line > fold.start && line <= fold.end) {
			if (document[fold.start].foldingState == FoldingState::foldable) {
				lineToFold = fold.start;
			}
		}
	}

	if (lineToFold != invalidLine) {
		document[line].foldingState = FoldingState::folded;
		forceUpdate = true;
	}
}


//
//	TextEditor::LineFold::unfoldAroundLine
//

void TextEditor::LineFold::unfoldAroundLine(Document& document, size_t line) {
	for (auto& fold : *this) {
		if (line > fold.start && line <= fold.end) {
			if (document[fold.start].foldingState == FoldingState::folded) {
				document[fold.start].foldingState = FoldingState::foldable;
				forceUpdate = true;
			}
		}
	}
}


//
//	TextEditor::LineFold::toggleAtLine
//

void TextEditor::LineFold::toggleAtLine(Document& document, size_t lineNo) {
	auto& line = document[lineNo];
	auto state = line.foldingState;

	if (state == FoldingState::foldable) {
		line.foldingState = FoldingState::folded;

		for (auto& fold : *this) {
			if (fold.start == lineNo) {
				for (size_t i = fold.start + 1; i <= fold.end; i++) {
					document[i].foldingState = FoldingState::hidden;
				}
			}
		}

		forceUpdate = true;

	} else if (state == FoldingState::folded) {
		line.foldingState = FoldingState::foldable;

		for (auto& fold : *this) {
			if (fold.start == lineNo) {
				for (size_t i = fold.start + 1; i <= fold.end; i++) {
					document[i].foldingState = FoldingState::visible;
				}
			}
		}

		forceUpdate = true;
	}
}


//
//	TextEditor::LineFold::unfoldAll
//

void TextEditor::LineFold::unfoldAll(Document& document) {
	for (auto& fold : *this) {
		if (document[fold.start].foldingState == FoldingState::folded) {
			document[fold.start].foldingState = FoldingState::foldable;
			forceUpdate = true;
		}
	}
}
