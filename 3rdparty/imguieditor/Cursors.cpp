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
//	TextEditor::Cursor::grow
//

void TextEditor::Cursor::grow(DocPos position) {
	if (position < getSelectionStart()) {
		if (end > start) {
			std::swap(end, start);
		}

		end = position;
		updated = true;

	} else if (position > getSelectionEnd()) {
		if (end < start) {
			std::swap(end, start);
		}

		end = position;
		updated = true;
	}
}


//
//	TextEditor::Cursor::adjustCoordinateForInsert
//

TextEditor::DocPos TextEditor::Cursor::adjustCoordinateForInsert(DocPos position, DocPos insertStart, DocPos insertEnd) {
	if (position.line == insertStart.line) {
		position.index += insertEnd.index - insertStart.index;
	}

	position.line += insertEnd.line - insertStart.line;
	return position;
}


//
//	TextEditor::Cursor::adjustForInsert
//

void TextEditor::Cursor::adjustForInsert(DocPos insertStart, DocPos insertEnd) {
	start = adjustCoordinateForInsert(start, insertStart, insertEnd);
	end = adjustCoordinateForInsert(end, insertStart, insertEnd);
}


//
//	TextEditor::Cursor::adjustCoordinateForDelete
//

TextEditor::DocPos TextEditor::Cursor::adjustCoordinateForDelete(DocPos position, DocPos deleteStart, DocPos deleteEnd) {
	if (deleteStart.line == deleteEnd.line) {
		if (position.line == deleteEnd.line) {
			position.index -= deleteEnd.index - deleteStart.index;
		}

	} else {
		position.line -= deleteEnd.line - deleteStart.line;

		if (position.line == deleteEnd.line) {
			position.index -= deleteEnd.index;
		}
	}

	return position;
}


//
//	TextEditor::Cursor::adjustForDelete
//

void TextEditor::Cursor::adjustForDelete(DocPos deleteStart, DocPos deleteEnd) {
	start = adjustCoordinateForDelete(start, deleteStart, deleteEnd);
	end = adjustCoordinateForDelete(end, deleteStart, deleteEnd);
}


//
//	TextEditor::Cursor::ensureNotHidden
//

void TextEditor::Cursor::ensureNotHidden(const Document& document) {
	if (document[start.line].foldingState == FoldingState::hidden) {
		auto line = start.line - 1;

		while (document[line].foldingState == FoldingState::hidden) {
			line--;
		}

		start.line = line;
		start.index = document[line].size();
		updated = true;
	}

	if (document[end.line].foldingState == FoldingState::hidden) {
		auto line = end.line - 1;

		while (document[line].foldingState == FoldingState::hidden) {
			line--;
		}

		end.line = line;
		end.index = document[line].size();
		updated = true;
	}
}


//
//	TextEditor::Cursors::reset
//

void TextEditor::Cursors::reset() {
	clear();
	main = 0;
	current = 0;
}


//
//	TextEditor::Cursors::setCursor
//

void TextEditor::Cursors::setCursor(DocPos cursorStart, DocPos cursorEnd) {
	reset();
	emplace_back(cursorStart, cursorEnd);
	front().setMain(true);
	front().setCurrent(true);
}


//
//	TextEditor::Cursors::addCursor
//

void TextEditor::Cursors::addCursor(DocPos start, DocPos end) {
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
	reset();
	emplace_back(DocPos(0, 0));
	front().setMain(true);
	front().setCurrent(true);
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

void TextEditor::Cursors::update(const Document& document) {
	// ensure cursors are not on hidden lines
	for (auto& cursor : *this) {
		cursor.ensureNotHidden(document);
	}

	if (anyHasUpdate()) {
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

			// find main and current cursor
			main = 0;
			current = 0;

			for (size_t c = 0; c < size(); c++) {
				if (at(c).isMain()) {
					main = c;

				} else if (at(c).isCurrent()) {
					current = c;
				}
			}
		}
	}
}


//
//	TextEditor::Cursors::adjustForInsert
//

void TextEditor::Cursors::adjustForInsert(iterator start, DocPos insertStart, DocPos insertEnd) {
	for (auto cursor = start + 1; cursor < end(); cursor++) {
		cursor->adjustForInsert(insertStart, insertEnd);
	}
}


//
//	TextEditor::Cursors::adjustForDelete
//

void TextEditor::Cursors::adjustForDelete(iterator start, DocPos deleteStart, DocPos deleteEnd) {
	for (auto cursor = start + 1; cursor < end(); cursor++) {
		cursor->adjustForDelete(deleteStart, deleteEnd);
	}
}
