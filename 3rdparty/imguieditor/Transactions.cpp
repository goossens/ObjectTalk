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
//	TextEditor::deleteText
//

void TextEditor::deleteText(std::shared_ptr<Transaction> transaction, Coordinate start, Coordinate end) {
	// update document and add transaction
	// this function does not touch the cursors
	auto text = document.getSectionText(start, end);
	document.deleteText(start, end);
	transaction->addDelete(start, end, text);
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

	for (auto i = transaction->rbegin(); i < transaction->rend(); i++) {
		if (i->type == Action::Type::insertText) {
			document.deleteText(i->start, i->end);

		} else {
			document.insertText(i->start, i->text);
		}
	}

	cursors = transaction->getBeforeState();
}


//
//	TextEditor::Transactions::redo
//

void TextEditor::Transactions::redo(Document& document, Cursors& cursors) {
	auto& transaction = at(undoIndex++);

	for (auto& action : *transaction) {
		if (action.type == Action::Type::insertText) {
			document.insertText(action.start, action.text);

		} else {
			document.deleteText(action.start, action.end);
		}
	}

	cursors = transaction->getAfterState();
}
