//	TextEditor - A syntax highlighting text editor for Dear ImGui.
//	Copyright (c) 2024-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui_internal.h"

#include "TextEditor.h"


//
//	TextEditor::AutoComplete::setConfig
//

void TextEditor::AutoComplete::setConfig(const AutoCompleteConfig* config) {
	if (config) {
		configuration = *config;

		// decode extra trigger and word characters
		auto decode = [](const std::string_view& text, std::vector<ImWchar>& codepoints) {
			codepoints.clear();

			for (auto i = text.begin(); i < text.end();) {
				ImWchar codepoint;
				i = CodePoint::read(i, text.end(), &codepoint);
				codepoints.emplace_back(codepoint);
			}
		};

		decode(configuration.triggerCharacters, triggerCodepoints);
		decode(configuration.wordCharacters, wordCodepoints);
		configured = true;

	} else {
		configured = false;
	}

	active = false;
}


//
//	TextEditor::AutoComplete::startTyping
//

bool TextEditor::AutoComplete::startTyping(Cursors& cursors, ImWchar character) {
	// a trigger character that isn't part of the search term ends the active session, so start a new one
	const auto restart = active && isTriggerCharacter(character) && !isWordCharacter(character);

	if ((!active || restart) && !requestActivation && configured && configuration.triggerOnTyping) {
		triggeredManually = false;
		cancel();
		start(cursors);
		return true;

	} else {
		return false;
	}
}


//
//	TextEditor::AutoComplete::startShortcut
//

bool TextEditor::AutoComplete::startShortcut(Cursors& cursors) {
	if (!active && !requestActivation && configured && configuration.triggerOnShortcut) {
		triggeredManually = true;
		start(cursors);
		return true;

	} else {
		return false;
	}
}


//
//	TextEditor::AutoComplete::cancel
//

void TextEditor::AutoComplete::cancel() {
	if (active) {
		requestDeactivation = true;
	}
}


//
//	renderSuggestion
//

static bool renderSuggestion(const std::string_view& suggestion, const std::string_view& searchTerm, float width, bool selected) {
	// custom widget to render an autocomplete suggestion in the style of Visual Studio Code
	auto glyphPos = ImGui::GetCursorScreenPos();
	const auto size = ImVec2(width, ImGui::GetFrameHeightWithSpacing());
	const auto clicked = ImGui::InvisibleButton("suggestion", size);

	auto drawList = ImGui::GetWindowDrawList();
	const auto font = ImGui::GetFont();
	const auto fontSize = ImGui::GetFontSize();
	const auto glyphWidth = ImGui::CalcTextSize("#").x;

	// highlight selected item
	if (selected) {
		drawList->AddRectFilled(glyphPos, glyphPos + size, ImGui::GetColorU32(ImGuiCol_Header));
	}

	// process all UTF-8 glyphs in suggestion
	glyphPos += ImGui::GetStyle().FramePadding;
	const auto suggestionEnd = suggestion.end();
	const auto searchTermEnd = searchTerm.end();
	auto i = TextEditor::CodePoint::skipBOM(suggestion.begin(), suggestionEnd);
	auto j = TextEditor::CodePoint::skipBOM(searchTerm.begin(), searchTermEnd);

	while (i < suggestionEnd) {
		// get next glyph from suggestion
		ImWchar codepoint;
		i = TextEditor::CodePoint::read(i, suggestionEnd, &codepoint);

		// highlight glyph in suggestion that match search term
		auto color = ImGui::GetColorU32(ImGuiCol_Text);

		if (j < searchTermEnd) {
			ImWchar searchCodePoint;
			const auto next = TextEditor::CodePoint::read(j, searchTermEnd, &searchCodePoint);

			if (TextEditor::CodePoint::toLower(searchCodePoint) == TextEditor::CodePoint::toLower(codepoint)) {
				color = ImGui::GetColorU32(ImGuiCol_TextLink);
				j = next;
			}
		}

		// render the glyph
		font->RenderChar(drawList, fontSize, glyphPos, color, codepoint);
		glyphPos.x += glyphWidth;
	}

	return clicked;
}


//
//	TextEditor::AutoComplete::render
//

bool TextEditor::AutoComplete::render(Document& document, Cursors& cursors, TypeSetter& typesetter, const Language* language, float textOffset, ImVec2 glyphSize) {
	// see if we need to activate autocomplete mode (after a previous session is closed)
	if (requestActivation && !active) {
		// apply popup delay
		if (std::chrono::system_clock::now() > activationTime) {
			// reset activation flag
			requestActivation = false;

			// capture locations
			startLocation = findWordStart(document, currentLocation);

			// update the autocomplete state
			updateState(document, language);

			// handle cases where autocomplete request is ignored
			if(state.inComment && !configuration.triggerInComments) {
				return false;
			}

			if(state.inString && !configuration.triggerInStrings) {
				return false;
			}

			// get initial list of suggestions from the app
			refreshSuggestions();

			// show autocomplete popup window
			ImGui::OpenPopup("AutoCompleteContextMenu");
			active = true;
		}
	}

	// only continue if autocomplete is active
	if (!active) {
		return false;
	}

	// see if cursor moved since last time
	const auto newLocation = cursors.getMain().getSelectionEnd();

	if (newLocation != currentLocation) {
		// see if we need to deactivate autocomplete because cursor is on new line
		if (newLocation.line != currentLocation.line) {
			requestDeactivation = true;

		} else {
			// see if cursor moved away from current word
			const auto newStart = findWordStart(document, newLocation);

			if (newStart == startLocation) {
				currentLocation = newLocation;

				// we deactivate autocomplete if the current location is the start
				if (currentLocation == startLocation) {
					requestDeactivation = true;

				} else {
					updateState(document, language);
					refreshSuggestions();
				}

			} else {
				requestDeactivation = true;
			}
		}
	}

	// open popup window
	bool result = false;
	const auto pos = typesetter.docPos2VisPos(document, currentLocation);

	ImGui::SetNextWindowPos(ImVec2(
		ImGui::GetCursorScreenPos().x + textOffset + pos.column * glyphSize.x,
		ImGui::GetCursorScreenPos().y + (pos.row + 1) * glyphSize.y));

	const auto suggestions = state.suggestions.size();

	// an empty result while typing dismisses silently; only a manual trigger earns the "no suggestions" feedback
	if (suggestions == 0 && !state.suggestionsPromise && !triggeredManually) {
		requestDeactivation = true;
	}

	const auto visibleSuggestions = (suggestions == 0) ? 1 : std::min(static_cast<size_t>(10), suggestions);
	const auto& style = ImGui::GetStyle();
	const auto height = ImGui::GetFrameHeightWithSpacing() * visibleSuggestions + style.WindowPadding.y * 2.0f;
	ImGui::SetNextWindowSize(ImVec2(configuration.suggestionWidth * glyphSize.x, height));

	const ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoNav;

	if (ImGui::BeginPopup("AutoCompleteContextMenu", flags)) {
		if (ImGui::IsWindowAppearing()) {
			ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
		}

		// deactivate popup (if requested)
		if (requestDeactivation) {
			ImGui::CloseCurrentPopup();
			requestDeactivation = false;
			active = false;

		} else {
			// do we have any suggestions
			if (suggestions) {
				const auto items = state.suggestions.size();
				auto scroll = false;

				// apply arrow keys to selected suggestion
				if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
					scroll = true;

					if (currentSelection == 0) {
						currentSelection = items - 1;

					} else {
						currentSelection--;
					}

				} else if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
					scroll = true;

					if (currentSelection == items - 1) {
						currentSelection = 0;

					} else {
						currentSelection++;
					}

				// use selected suggestion if user hit tab of return
				} else if (ImGui::IsKeyPressed(ImGuiKey_Tab) || ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter)) {
					requestDeactivation = true;
					result = true;

				} else if (configuration.autoInsertSingleSuggestions && triggeredManually && state.suggestions.size() == 1) {
					requestDeactivation = true;
					result = true;
				}

				// render suggestions (or their labels)
				const auto& texts = state.labels.size() == items ? state.labels : state.suggestions;

				// highlight the text that will be replaced (the callback could have moved its start)
				const auto highlightTerm = document.getSectionText(getReplacementStart(), currentLocation);

				for (size_t i = 0; i < items; i++) {
					// ensure unique ID
					ImGui::PushID(static_cast<int>(i));

					// scroll list to selected item (if required)
					const auto selected = i == currentSelection;

					if (scroll && selected) {
						ImGui::SetScrollHereY(0.5f);
					}

					if (renderSuggestion(texts[i], highlightTerm, ImGui::GetContentRegionAvail().x, selected)) {
						// user clicked on a suggestion, use it
						currentSelection = i;
						requestDeactivation = true;
						result = true;
					}

					ImGui::PopID();
				}

			} else {
				ImGui::TextUnformatted(configuration.noSuggestionsLabel.c_str());
			}
		}

		ImGui::EndPopup();

	} else {
		requestDeactivation = false;
		active = false;
	}

	return result;
}


//
//	TextEditor::AutoComplete::setSuggestions
//

void TextEditor::AutoComplete::setSuggestions(const std::vector<std::string>& suggestions, const std::vector<std::string>& labels) {
	state.suggestions = suggestions;
	state.labels = labels;
	currentSelection = 0;
}


//
//	TextEditor::AutoComplete::isTriggerCharacter
//

bool TextEditor::AutoComplete::isTriggerCharacter(ImWchar codepoint) const {
	return std::find(triggerCodepoints.begin(), triggerCodepoints.end(), codepoint) != triggerCodepoints.end();
}


//
//	TextEditor::AutoComplete::getReplacementStart
//

TextEditor::DocPos TextEditor::AutoComplete::getReplacementStart() const {
	// use the replacement start (which could have been changed by the callback) if it is valid
	const auto start = state.replacementStart;

	if (start.line == currentLocation.line && start.index <= currentLocation.index) {
		return start;

	} else {
		return startLocation;
	}
}


//
//	TextEditor::AutoComplete::getReplacementEnd
//

TextEditor::DocPos TextEditor::AutoComplete::getReplacementEnd(const Document& document) const {
	// use the replacement end (which could have been changed by the callback) if it is valid
	const auto end = state.replacementEnd;

	if (end.line == currentLocation.line && end.index >= currentLocation.index && end.index <= document[end.line].size()) {
		return end;

	} else {
		return findWordEnd(document, currentLocation);
	}
}


//
//	TextEditor::AutoComplete::isSpecialKeyPressed
//

bool TextEditor::AutoComplete::isSpecialKeyPressed() {
	for (const auto key : {ImGuiKey_Tab, ImGuiKey_Enter, ImGuiKey_KeypadEnter, ImGuiKey_UpArrow, ImGuiKey_DownArrow}) {
		if (ImGui::IsKeyPressed(key)) {
			return true;
		}
	}

	return false;
}


//
//	TextEditor::AutoComplete::start
//

void TextEditor::AutoComplete::start(Cursors& cursors) {
	// request start of autocomplete mode (can't be done here as the Dear ImGui context might not be right)
	requestActivation = true;
	currentLocation = cursors.getMain().getSelectionEnd();
	activationTime = std::chrono::system_clock::now() + configuration.triggerDelay;
}


//
//	TextEditor::AutoComplete::updateState
//

void TextEditor::AutoComplete::updateState(Document& document, const Language* language) {
	state.searchTerm = document.getSectionText(startLocation, currentLocation);

	if (currentLocation.index == 0) {
		state.inIdentifier = false;
		state.inNumber = false;

		const auto lineState = document[currentLocation.line].state;
		state.inComment = lineStateInComment(lineState);
		state.inString = lineStateInString(lineState);

	} else {
		const auto color = document.getColor(document.getLeft(currentLocation));
		state.inIdentifier = color == Color::identifier || color == Color::knownIdentifier;
		state.inNumber = color == Color::number;
		state.inComment = color == Color::comment;
		state.inString = color == Color::string;
	}

	state.searchTermStart = startLocation;
	state.searchTermEnd = currentLocation;
	state.replacementStart = startLocation;
	state.replacementEnd = findWordEnd(document, currentLocation);

	state.language = language;
	state.userData = configuration.userData;
}


//
//	TextEditor::AutoComplete::refreshSuggestions
//

void TextEditor::AutoComplete::refreshSuggestions() {
	// labels only apply to the suggestions from the call that set them
	state.labels.clear();

	// populate suggestion list through callback (or clear it if there is none)
	if (configuration.callback) {
		configuration.callback(state);

	} else {
		state.suggestions.clear();
	}

	currentSelection = 0;
}


//
//	TextEditor::AutoComplete::isWordCharacter
//

bool TextEditor::AutoComplete::isWordCharacter(ImWchar codepoint) const {
	return CodePoint::isWord(codepoint) || std::find(wordCodepoints.begin(), wordCodepoints.end(), codepoint) != wordCodepoints.end();
}


//
//	TextEditor::AutoComplete::findWordStart
//

TextEditor::DocPos TextEditor::AutoComplete::findWordStart(const Document& document, DocPos from) const {
	// like Document::findWordStart but with the extra word characters
	const auto& line = document[from.line];
	auto index = from.index;

	while (index > 0 && isWordCharacter(line[index - 1].codepoint)) {
		index--;
	}

	return DocPos(from.line, index);
}


//
//	TextEditor::AutoComplete::findWordEnd
//

TextEditor::DocPos TextEditor::AutoComplete::findWordEnd(const Document& document, DocPos from) const {
	// like Document::findWordEnd but with the extra word characters
	const auto& line = document[from.line];
	auto index = from.index;

	while (index < line.size() && isWordCharacter(line[index].codepoint)) {
		index++;
	}

	return DocPos(from.line, index);
}
