//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtObjectTalkEditor.h"
#include "OtObjectTalkLanguage.h"


//
//	OtObjectTalkEditor::OtObjectTalkEditor
//

OtObjectTalkEditor::OtObjectTalkEditor() {
	// configure language
	auto language = OtObjectTalkLanguage::getDefinition();
	editor.SetLanguage(language);
	diff.SetLanguage(language);

	// activate autocomplete
	TextEditor::AutoCompleteConfig config;

	config.callback = [this](TextEditor::AutoCompleteState& state) {
		trie.findSuggestions(state.suggestions, state.searchTerm);
	};

	editor.SetAutoCompleteConfig(&config);
}


//
//	OtObjectTalkEditor::clear
//

void OtObjectTalkEditor::clear() {
	// reset editor and autocomplete
	OtTextEditor::clear();
	trie.clear();
}


//
//	OtObjectTalkEditor::load
//

void OtObjectTalkEditor::load() {
	// load the document
	OtTextEditor::load();

	// rebuild autocomplete word list
	trie.clear();

	// add language words
	auto language = editor.GetLanguage();
	for (auto& word : language->keywords) { trie.insert(word); }
	for (auto& word : language->declarations) { trie.insert(word); }
	for (auto& word : language->identifiers) { trie.insert(word); }

	// add all words in document
	editor.IterateIdentifiers([this](const std::string& identifier) {
		trie.insert(identifier);
	});
}


//
//	OtObjectTalkEditor::highlightError
//

void OtObjectTalkEditor::highlightError(size_t line, const std::string& error) {
	// line number is zero-base in text editor
	int editorLine = static_cast<int>(line - 1);
	editor.AddMarker(editorLine, 0, IM_COL32(128, 0, 32, 128), "", error);
	editor.SetCursor(editorLine, 0);
	editor.ScrollToLine(editorLine, TextEditor::Scroll::alignMiddle);
}


//
//	OtObjectTalkEditor::clearError
//

void OtObjectTalkEditor::clearError() {
	editor.ClearMarkers();
}
