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
//	TextEditor::Cursor::adjustCoordinateForInsert
//

TextEditor::Coordinate TextEditor::Cursor::adjustCoordinateForInsert(Coordinate coordinate, Coordinate start, Coordinate end) {
	coordinate.line += end.line - start.line;

	if (end.line == coordinate.line) {
		coordinate.column += end.column;
	}

	return coordinate;
}


//
//	TextEditor::Cursor::adjustForInsert
//

void TextEditor::Cursor::adjustForInsert(Coordinate insertStart, Coordinate insertEnd) {
	start = adjustCoordinateForInsert(start, insertStart, insertEnd);
	end = adjustCoordinateForInsert(end, insertStart, insertEnd);
}


//
//	TextEditor::Cursor::adjustCoordinateForDelete
//

TextEditor::Coordinate TextEditor::Cursor::adjustCoordinateForDelete(Coordinate coordinate, Coordinate start, Coordinate end) {
	coordinate.line -= end.line - start.line;

	if (end.line == coordinate.line) {
		coordinate.column -= end.column;
	}

	return coordinate;
}


//
//	TextEditor::Cursor::adjustForDelete
//

void TextEditor::Cursor::adjustForDelete(Coordinate deleteStart, Coordinate deleteEnd) {
	start = adjustCoordinateForDelete(start, deleteStart, deleteEnd);
	end = adjustCoordinateForDelete(end, deleteStart, deleteEnd);
}


//
//	TextEditor::Cursors::setCursor
//

void TextEditor::Cursors::setCursor(Coordinate start, Coordinate end) {
	clear();
	emplace_back(start, end);
	front().setMain(true);
	front().setCurrent(true);
	main = 0;
	current = 0;
}


//
//	TextEditor::Cursors::addCursor
//

void TextEditor::Cursors::addCursor(Coordinate start, Coordinate end) {
	at(current).setCurrent(false);
	emplace_back(start, end);
	back().setCurrent(true);
	current = size() - 1;
}


//
//	TextEditor::Cursors::anyHasSelection
//

bool TextEditor::Cursors::anyHasSelection() const {
	for (auto cursor = begin(); cursor < end(); cursor++) {
		if (cursor->hasSelection()) {
			return true;
		}
	}

	return false;
}


//
//	TextEditor::Cursors::allHaveSelection
//

bool TextEditor::Cursors::allHaveSelection() const {
	for (auto cursor = begin(); cursor < end(); cursor++) {
		if (!cursor->hasSelection()) {
			return false;
		}
	}

	return true;
}


//
//	TextEditor::Cursors::anyHasUpdate
//

bool TextEditor::Cursors::anyHasUpdate() const {
	for (auto cursor = begin(); cursor < end(); cursor++) {
		if (cursor->isUpdated()) {
			return true;
		}
	}

	return false;
}


//
//	TextEditor::Cursors::clearAll
//

void TextEditor::Cursors::clearAll() {
	clear();
	emplace_back(Coordinate(0, 0));
	front().setMain(true);
	front().setCurrent(true);
	main = 0;
	current = 0;
}


//
//	TextEditor::Cursors::clearAdditional
//

void TextEditor::Cursors::clearAdditional(bool reset) {
	for (auto cursor = begin(); cursor < end();) {
		if (cursor->isMain()) {
			cursor++;

		} else {
			cursor = erase(cursor);
		}
	}

	main = 0;
	current = 0;
	front().setCurrent(true);

	if (reset) {
		front().resetToEnd();
	}
}


//
//	TextEditor::Cursors::clearUpdated
//

void TextEditor::Cursors::clearUpdated() {
	for (auto cursor = begin(); cursor < end(); cursor++) {
		cursor->setUpdated(false);
	}
}


//
//	TextEditor::Cursors::update
//

void TextEditor::Cursors::update() {
	// reset update flags
	clearUpdated();

	//  only sort and potential merge when we have multiple cursors
	if (hasMultiple()) {
		// sort cursors
		std::sort(begin(), end(), [](Cursor& a, Cursor& b) {
			return a.getSelectionStart() < b.getSelectionStart();
		});

		// merge cursors
		for (auto cursor = rbegin(); cursor < rend() - 1;) {
			auto previous = cursor + 1;

			if (previous->getSelectionEnd() >= cursor->getSelectionEnd()) {
				if (cursor->isMain()) {
					previous->setMain(true);
				}

				if (cursor->isCurrent()) {
					previous->setCurrent(true);
				}

				erase((++cursor).base());

			} else if (previous->getSelectionEnd() > cursor->getSelectionStart()) {
				if (cursor->getInteractiveEnd() < cursor->getInteractiveStart()) {
					previous->update(cursor->getSelectionEnd(), previous->getSelectionStart());

				} else {
					previous->update(previous->getSelectionStart(), cursor->getSelectionEnd());
				}

				if (cursor->isMain()) {
					previous->setMain(true);
				}

				if (cursor->isCurrent()) {
					previous->setCurrent(true);
				}

				erase((++cursor).base());

			} else {
				cursor++;
			}
		}

		// find current cursor
		for (auto c = 0; c < size(); c++) {
			if (at(c).isCurrent()) {
				current = c;
			}
		}
	}
}


//
//	TextEditor::Cursors::adjustForInsert
//

void TextEditor::Cursors::adjustForInsert(iterator start, Coordinate insertStart, Coordinate insertEnd) {
	for (auto cursor = start + 1; cursor < end(); cursor++) {
		cursor->adjustForInsert(insertStart, insertEnd);
	}
}


//
//	TextEditor::Cursors::adjustForDelete
//

void TextEditor::Cursors::adjustForDelete(iterator start, Coordinate deleteStart, Coordinate deleteEnd) {
	for (auto cursor = start + 1; cursor < end(); cursor++) {
		cursor->adjustForDelete(deleteStart, deleteEnd);
	}
}
