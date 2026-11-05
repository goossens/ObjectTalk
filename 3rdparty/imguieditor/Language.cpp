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
//	TextEditor::SetLanguage
//

void TextEditor::SetLanguage(const Language* language) {
	config.language = language;

	if (languageChangeCallback) {
		languageChangeCallback();
	}
}


//
//	TextEditor::RegisterLanguage
//

void TextEditor::RegisterLanguage(const std::string name, std::function<const Language*()> creator, const std::vector<std::string> extensions) {
	builtinLanguages.emplace_back(name);
	builtinLanguagesIndex.emplace(name);
	std::sort(builtinLanguages.begin(), builtinLanguages.end());
	builtinLanguageCreators[name] = creator;

	for (auto& extension : extensions) {
		builtinLanguageExtensions[extension] = creator;
	}
}


//
//	TextEditor::LookupLanguage
//

const TextEditor::Language* TextEditor::LookupLanguage(const std::string& name) {
	if (!languagesRegistered) {
		registerLanguages();
		languagesRegistered = true;
	}

	if (builtinLanguagesIndex.find(name) == builtinLanguagesIndex.end()) {
		return nullptr;

	} else {
		return builtinLanguageCreators[name]();
	}
}


//
//	TextEditor::LookupLanguageByExtension
//

const TextEditor::Language* TextEditor::LookupLanguageByExtension(const std::string& extension) {
	if (!languagesRegistered) {
		registerLanguages();
		languagesRegistered = true;
	}

	if (builtinLanguageExtensions.find(extension) == builtinLanguageExtensions.end()) {
		return nullptr;

	} else {
		return builtinLanguageExtensions[extension]();
	}
}


//
//	TextEditor::GetRegisteredLanguages
//

const std::vector<std::string> TextEditor::GetRegisteredLanguages() {
	if (!languagesRegistered) {
		registerLanguages();
		languagesRegistered = true;
	}

	return builtinLanguages;
}


//
//	Include language definitions
//

#include "TextEditorLanguages.h"
