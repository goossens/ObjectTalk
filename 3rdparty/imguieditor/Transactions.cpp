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
	version = 0;
}


//
//	TextEditor::Transactions::add
//

void TextEditor::Transactions::add(std::shared_ptr<Transaction> transaction) {
	resize(undoIndex);
	push_back(transaction);
	undoIndex++;
	version++;

	if (callback) {
		std::vector<Change> changes;

		for (auto& action : *transaction) {
			auto& change = changes.emplace_back();
			change.insert = action.type == Action::Type::insertText;
			change.start = action.start;
			change.end = action.end;
			change.text = action.text;
		}

		callback(changes);
	}
}


//
//	TextEditor::Transactions::undo
//

void TextEditor::Transactions::undo(Config& config, Document& document, Cursors& cursors) {
	auto transaction = at(--undoIndex);

	for (auto action = transaction->rbegin(); action < transaction->rend(); action++) {
		if (action->type == Action::Type::insertText) {
			document.deleteText(config, action->start, action->end);

		} else {
			document.insertText(config, action->start, action->text);
		}
	}

	cursors = transaction->getBeforeState();
	version++;

	if (callback) {
		std::vector<Change> changes;

		for (auto& action : *transaction) {
			auto& change = changes.emplace_back();
			change.insert = action.type == Action::Type::deleteText;
			change.start = action.start;
			change.end = action.end;
			change.text = action.text;
		}

		callback(changes);
	}
}


//
//	TextEditor::Transactions::redo
//

void TextEditor::Transactions::redo(Config& config, Document& document, Cursors& cursors) {
	auto transaction = at(undoIndex++);

	for (auto action = transaction->begin(); action < transaction->end(); action++) {
		if (action->type == Action::Type::insertText) {
			document.insertText(config, action->start, action->text);

		} else {
			document.deleteText(config, action->start, action->end);
		}
	}

	cursors = transaction->getAfterState();
	version++;

	if (callback) {
		std::vector<Change> changes;

		for (auto& action : *transaction) {
			auto& change = changes.emplace_back();
			change.insert = action.type == Action::Type::insertText;
			change.start = action.start;
			change.end = action.end;
			change.text = action.text;
		}

		callback(changes);
	}
}
