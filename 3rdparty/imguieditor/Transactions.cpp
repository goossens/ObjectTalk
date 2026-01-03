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
//	TextEditor::Transactions::reset
//

void TextEditor::Transactions::reset() {
	clear();
	undoIndex = 0;
}


//
//	TextEditor::Transactions::add
//

void TextEditor::Transactions::add(std::shared_ptr<Transaction> transaction) {
	resize(undoIndex);
	push_back(transaction);
	undoIndex++;
}


//
//	TextEditor::Transactions::undo
//

void TextEditor::Transactions::undo(Document& document, Cursors& cursors) {
	auto& transaction = at(--undoIndex);

	for (auto action = transaction->rbegin(); action < transaction->rend(); action++) {
		if (action->type == Action::Type::insertText) {
			document.deleteText(action->start, action->end);

		} else {
			document.insertText(action->start, action->text);
		}
	}

	cursors = transaction->getBeforeState();
}


//
//	TextEditor::Transactions::redo
//

void TextEditor::Transactions::redo(Document& document, Cursors& cursors) {
	auto& transaction = at(undoIndex++);

	for (auto action = transaction->rbegin(); action < transaction->rend(); action++) {
		if (action->type == Action::Type::insertText) {
			document.insertText(action->start, action->text);

		} else {
			document.deleteText(action->start, action->end);
		}
	}

	cursors = transaction->getAfterState();
}
