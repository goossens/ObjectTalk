//	TextEditor - A syntax highlighting text editor for ImGui
//	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cctype>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

#include "CodePoint.h"

#include "TextEditor.h"


//
//	TextEditor::setText
//

void TextEditor::setText(const std::string& text) {
	// load text into document and reset transactions and cursors
	document.setText(text);
	transactions.clear();
	cursors.clearAll();
}


//
//	TextEditor::render
//

void TextEditor::render(const char* title, const ImVec2& size, bool border) {
	// update color palette (if required)
	if (paletteAlpha != ImGui::GetStyle().Alpha) {
		updatePalette();
	}

	// get font information and determine start of text
	font = ImGui::GetFont();
	fontSize = ImGui::GetFontSize();
	glyphSize = ImVec2(font->CalcTextSizeA(fontSize, FLT_MAX, -1.0f, "#").x, ImGui::GetTextLineHeightWithSpacing() * lineSpacing);
	textStart = leftMargin * glyphSize.x;
	auto tabSize = document.getTabSize();

	// adjust text start in case line numbers are shown
	if (showLineNumbers) {
		int digits = static_cast<int>(std::log10(document.lines() + 1) + 1.0f);
		textStart += glyphSize.x * (digits + lineNumberMargin);
	}

	// set style and start a new child window
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertU32ToFloat4(getColor(Color::background)));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	int longestLine = document.maxColumn();
	ImGui::SetNextWindowContentSize({textStart + longestLine * glyphSize.x + cursorWidth, document.lines() * glyphSize.y});
	ImGui::BeginChild(title, size, border, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoNavInputs);

	// handle keyboard and mouse inputs
	handleKeyboardInputs();
	handleMouseInteractions();
	cursors.update();

	// ensure cursor is visible (if requested)
	if (ensureCursorIsVisible) {
		makeCursorVisible();
		ensureCursorIsVisible = false;
	}

	// determine view parameters
	float scrollbarSize = ImGui::GetStyle().ScrollbarSize;
	visibleHeight = ImGui::GetWindowHeight() - ((longestLine >= visibleColumns) ? scrollbarSize : 0.0f);
	visibleLines = std::max(static_cast<int>(std::ceil(visibleHeight / glyphSize.y)), 0);
	firstVisibleLine = std::max(static_cast<int>(std::floor(ImGui::GetScrollY() / glyphSize.y)), 0);
	lastVisibleLine = static_cast<int>(std::ceil((ImGui::GetScrollY() + visibleHeight) / glyphSize.y));

	visibleWidth = ImGui::GetWindowWidth() - textStart - ((document.lines() >= visibleLines) ? scrollbarSize : 0.0f);
	visibleColumns = std::max(static_cast<int>(std::ceil(visibleWidth / glyphSize.x)), 0);
	firstVisibleColumn = (std::max(static_cast<int>(std::floor(ImGui::GetScrollX() / glyphSize.x)), 0) / tabSize) * tabSize;
	lastVisibleColumn = ((static_cast<int>(std::ceil((ImGui::GetScrollX() + visibleWidth) / glyphSize.x)) + 1) / tabSize) * tabSize;

	// update cursor animation timer
	auto io = ImGui::GetIO();
	cursorAnimationTimer = std::fmod(cursorAnimationTimer + io.DeltaTime, 1.0f);

	// render all visible lines
	auto drawList = ImGui::GetWindowDrawList();
	ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
	ImVec2 lineScreenPos = cursorScreenPos + ImVec2(textStart, firstVisibleLine * glyphSize.y);

	for (int i = firstVisibleLine; i <= lastVisibleLine && i < document.lines(); i++) {
		// render single line
		auto& line = document[i];

		// draw selections
		for (auto& cursor : cursors) {
			if (cursor.hasSelection()) {
				int rectStart = -1;
				int rectEnd = -1;
				Coordinate lineStart{i, 0};
				Coordinate lineEnd{i, document.maxColumn(line)};
				auto selectionStart = cursor.getSelectionStart();
				auto selectionEnd = cursor.getSelectionEnd();

				// see if this cursor overlaps this line
				if (selectionStart <= lineEnd) {
					rectStart = (selectionStart > lineStart ? selectionStart.column : 0) * glyphSize.x;
				}

				if (selectionEnd > lineStart) {
					rectEnd = (selectionEnd < lineEnd ? selectionEnd.column : lineEnd.column) * glyphSize.x;
				}

				// draw selection background if it does
				if (rectStart != -1 && rectEnd != -1) {
					drawList->AddRectFilled(
						ImVec2{lineScreenPos.x + rectStart, lineScreenPos.y},
						ImVec2{lineScreenPos.x + rectEnd, lineScreenPos.y + glyphSize.y},
						getColor(Color::selection));
				}
			}
		}

		// draw colored line glyphs
		auto column = firstVisibleColumn;
		auto index = document.getIndex(line, column);
		auto glyphs = line.glyphs();

		while (index < glyphs && column <= lastVisibleColumn) {
			auto& glyph = line[index];
			auto character = glyph.character;
			ImVec2 glyphPos{lineScreenPos.x + column * glyphSize.x, lineScreenPos.y};

			if (character == '\t') {
				if (showWhitespaces) {
					const auto x1 = glyphPos.x + glyphSize.x * 0.3f;
					const auto y = glyphPos.y + fontSize * 0.5f;
					const auto x2 = glyphPos.x + glyphSize.x;

					ImVec2 p1, p2, p3, p4;
					p1 = ImVec2(x1, y);
					p2 = ImVec2(x2, y);
					p3 = ImVec2(x2 - fontSize * 0.16f, y - fontSize * 0.16f);
					p4 = ImVec2(x2 - fontSize * 0.16f, y + fontSize * 0.16f);

					drawList->AddLine(p1, p2, getColor(Color::whitespace));
					drawList->AddLine(p2, p3, getColor(Color::whitespace));
					drawList->AddLine(p2, p4, getColor(Color::whitespace));
				}

			} else if (character == ' ') {
				if (showWhitespaces) {
					const auto x = glyphPos.x + glyphSize.x * 0.5f;
					const auto y = glyphPos.y + fontSize * 0.5f;
					drawList->AddCircleFilled(ImVec2(x, y), 1.5f, getColor(Color::whitespace), 4);
				}

			} else {
				font->RenderChar(drawList, fontSize, glyphPos, getColor(glyph.color), static_cast<ImWchar>(character));
			}

			index++;
			column += (character == '\t') ? tabSize - (column % tabSize) : 1;
		}

		// draw cursors (if required)
		if (ImGui::IsWindowFocused() && (!io.ConfigInputTextCursorBlink || cursorAnimationTimer < 0.5f)) {
			for (auto& cursor : cursors) {
				auto pos = cursor.getInteractiveEnd();

				if (pos.line == i) {
					auto x = lineScreenPos.x + pos.column * glyphSize.x;
					drawList->AddRectFilled(ImVec2{x, lineScreenPos.y}, ImVec2{x + cursorWidth, lineScreenPos.y + glyphSize.y}, getColor(Color::cursor));
				}
			}
		}

		lineScreenPos.y += glyphSize.y;
	}

	// draw line numbers (if required)
	if (showLineNumbers) {
		// erase background in case we are scrolling horizontally
		if (ImGui::GetScrollX() > 0.0f) {
			drawList->AddRectFilled(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImVec2(textStart, ImGui::GetWindowSize().y), getColor(Color::background));
		}

		auto curserLine = cursors.getCurrent().getInteractiveEnd().line;
		auto lineNumberEnd = ImGui::GetWindowPos().x + textStart;

		for (int i = firstVisibleLine; i <= lastVisibleLine && i < document.lines(); i++) {
				int digits = static_cast<int>(std::log10(i + 1) + 1.0f);
				auto lineNumberWidth = (digits + lineNumberMargin) * glyphSize.x;
				Color foreground =  i == curserLine ? Color::currentLineNumber : Color::lineNumber;
				drawList->AddText(ImVec2(lineNumberEnd - lineNumberWidth, cursorScreenPos.y + i * glyphSize.y), getColor(foreground), std::to_string(i + 1).c_str());
		}
	}

	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}


//
//	TextEditor::handleKeyboardInputs
//

void TextEditor::handleKeyboardInputs() {
	if (ImGui::IsWindowFocused()) {
		ImGuiIO& io = ImGui::GetIO();
		io.WantCaptureKeyboard = true;
		io.WantTextInput = true;

		// get current key modifier state
		auto shift = ImGui::IsKeyDown(ImGuiMod_Shift);
		auto ctrl = ImGui::IsKeyDown(ImGuiMod_Ctrl);
		auto alt = ImGui::IsKeyDown(ImGuiMod_Alt);

		auto isNoModifiers = !ctrl && !shift && !alt;
		auto isShortcut = ctrl && !shift && !alt;
		auto isShiftShortcut = ctrl && shift && !alt;
		auto isOptionalShiftShortcut = ctrl && !alt;
		auto isAltOnly = !ctrl && !shift && alt;
		auto isShiftOnly = !ctrl && shift && !alt;
		auto isOptionalShift = !ctrl && !alt;
		auto isOptionalAlt = !ctrl && !shift;
		auto isOptionalAltShift = !ctrl;

		// cursor movements and selections
		if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_UpArrow)) { moveUp(1, shift); }
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_DownArrow)) { moveDown(1, shift); }
		else if (isOptionalAltShift && ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) { moveLeft(shift, alt); }
		else if (isOptionalAltShift && ImGui::IsKeyPressed(ImGuiKey_RightArrow)) { moveRight(shift, alt); }
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_PageUp)) { moveUp(visibleLines - 2, shift); }
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_PageDown)) { moveDown(visibleLines - 2, shift); }
		else if (isOptionalShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_UpArrow)) { moveToTop(shift); }
		else if (isOptionalShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_Home)) { moveToTop(shift); }
		else if (isOptionalShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_DownArrow)) { moveToBottom(shift); }
		else if (isOptionalShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_End)) { moveToBottom(shift); }
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_Home)) { moveToStartOfLine(shift); }
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_End)) { moveToEndOfLine(shift); }
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_A)) { selectAll(); }
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_D) && cursors.currentCursorHasSelection()) { addNextOccurrence(); }

		// clipboard operations
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_X)) { cut(); }
		else if (isShiftOnly && ImGui::IsKeyPressed(ImGuiKey_Delete)) { cut(); }
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_Insert)) { copy(); }
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_C)) { copy() ;}

		else if (!readOnly && isShiftOnly && ImGui::IsKeyPressed(ImGuiKey_Insert)) { paste(); }
		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_V)) { paste(); }
		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_Z)) { undo(); }
		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_Y)) { redo(); }
		else if (!readOnly && isShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_Z)) { redo(); }

		// remove text
		else if (!readOnly && isOptionalAlt && ImGui::IsKeyPressed(ImGuiKey_Delete)) { handleDelete(alt); }
		else if (!readOnly && isOptionalAlt && ImGui::IsKeyPressed(ImGuiKey_Backspace)) { handleBackspace(alt); }
		else if (!readOnly && isShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_K)) { removeSelectedLines(); }

		// text manipulation
		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_LeftBracket)) { deindentLine(); }
		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_RightBracket)) { indentLine(); }
		else if (!readOnly && isAltOnly && ImGui::IsKeyPressed(ImGuiKey_UpArrow)) { moveUpLines(); }
		else if (!readOnly && isAltOnly && ImGui::IsKeyPressed(ImGuiKey_DownArrow)) { moveDownLines(); }
		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_Slash)) { toggleComment(); }

		// handle new line
		else if (!readOnly && isNoModifiers && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))) { handleCharacter('\n'); }
		else if (!readOnly && isShortcut && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))) { insertLineBelow(); }
		else if (!readOnly && isShiftShortcut && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))) { insertLineAbove(); }

		// handle tabs
		else if (!readOnly && isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_Tab)) {
			if (cursors.anyHasSelection()) {
				if (shift) {
					deindentLine();

				} else {
					indentLine();
				}

			} else {
				handleCharacter('\t');
			}
		}

		// handle regular text
		if (!readOnly && !io.InputQueueCharacters.empty()) {
			for (int i = 0; i < io.InputQueueCharacters.Size; i++) {
				auto character = io.InputQueueCharacters[i];

				if (character == '\n' || character >= 32)
					handleCharacter(character);
			}

			io.InputQueueCharacters.resize(0);
		}
	}
}


//
//	TextEditor::handleMouseInteractions
//

void TextEditor::handleMouseInteractions() {
	// pan with dragging middle mouse button
	if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
		ImVec2 mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
		ImGui::SetScrollX(ImGui::GetScrollX() - mouseDelta.x);
		ImGui::SetScrollY(ImGui::GetScrollY() - mouseDelta.y);
		ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);

	// ignore other interactions when the editor is not hovered
	} else if (ImGui::IsWindowHovered()) {
		auto io = ImGui::GetIO();
		ImVec2 mousePos = ImGui::GetMousePos() - ImGui::GetCursorScreenPos();
		bool overLineNumbers = showLineNumbers && (mousePos.x - ImGui::GetScrollX() < textStart);

		auto mouseCoord = document.normalizeCoordinate(Coordinate(
			std::max(0, static_cast<int>(floor(mousePos.y / glyphSize.y))),
			std::max(0, static_cast<int>(floor((mousePos.x - textStart) / glyphSize.x)))
		));

		// show text cursor if required
		if (ImGui::IsWindowFocused() && !overLineNumbers) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
		}

		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
			// update selection with dragging left mouse button
			io.WantCaptureMouse = true;

			if (overLineNumbers) {
				auto& cursor = cursors.getCurrent();
				auto start = Coordinate(mouseCoord.line, 0);
				auto end = document.getDown(start);
				cursors.updateCursor(cursor.getInteractiveEnd() < cursor.getInteractiveStart() ? start : end);

			} else {
				cursors.updateCursor(mouseCoord);
			}

		} else {
			// handle left mouse button actions
			auto click = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
			auto doubleClick = ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
			auto now = static_cast<float>(ImGui::GetTime());
			auto tripleClick = click && !doubleClick && (lastClickTime != -1.0f && (now - lastClickTime) < io.MouseDoubleClickTime);

			if (click || doubleClick || tripleClick) {
				lastClickTime = tripleClick ? -1.0f : now;
			}

			if (tripleClick) {
				// left mouse button triple click
				if (!overLineNumbers) {
					auto start = document.getStartOfLine(mouseCoord);
					auto end = document.getDown(start);
					cursors.updateCursor(start, end);
				}

			} else if (doubleClick) {
				// left mouse button double click
				if (!overLineNumbers) {
					auto start = document.findWordStart(mouseCoord);
					auto end = document.findWordEnd(mouseCoord);
					cursors.updateCursor(start, end);
				}

			} else if (click) {
				// left mouse button single click
				auto extendCursor = ImGui::IsKeyDown(ImGuiMod_Shift);

#if __APPLE__
				auto addCursor = ImGui::IsKeyDown(ImGuiMod_Alt);
#else
				auto addCursor = ImGui::IsKeyDown(ImGuiMod_Ctrl);
#endif

				// handle mouse clicks over line numbers
				if (overLineNumbers) {
					auto start = Coordinate(mouseCoord.line, 0);
					auto end = document.getDown(start);

					if (extendCursor) {
						auto& cursor = cursors.getCurrent();
						cursors.updateCursor(cursor.getInteractiveEnd() < cursor.getInteractiveStart() ? start : end);

					} else if (addCursor) {
						cursors.addCursor(start, end);

					} else {
						cursors.setCursor(start, end);
					}

				// handle mouse clicks over text
				} else {
					if (extendCursor) {
						cursors.updateCursor(mouseCoord);

					} else if (addCursor) {
						cursors.addCursor(mouseCoord);

					} else {
						cursors.setCursor(mouseCoord);
					}
				}
			}
		}
	}
}


//
//	TextEditor::selectAll
//

void TextEditor::selectAll() {
	moveToTop(false);
	moveToBottom(true);
}


//
//	TextEditor::selectLine
//

void TextEditor::selectLine(int line) {
	Coordinate start{line, 0};
	moveTo(start, false);
	moveTo(document.getDown(start), true);
}


//
//	TextEditor::selectLines
//

void TextEditor::selectLines(int startLine, int endLine) {
	Coordinate start{startLine, 0};
	moveTo(start, false);
	moveTo(document.getDown(start, endLine - startLine + 1), true);
}


//
//	TextEditor::cut
//

void TextEditor::cut() {
	// copy selection to clipboard and remove it
	copy();
	auto transaction = startTransaction();
	deleteTextAtCursors(transaction);
	cursors.getCurrent().resetToStart();
	endTransaction(transaction);
}


//
//	TextEditor::copy
//

void TextEditor::copy() const {
	std::string text;

	if (cursors.anyHasSelection()) {
		for (auto& cursor : cursors) {
			if (text.size()) {
				text += "\n";
			}

			if (cursor.hasSelection()) {
				text += document.getSectionText(cursor.getSelectionStart(), cursor.getSelectionEnd());

			} else {
				text += document.getLineText(cursor.getSelectionStart().line);
			}
		}

	} else {
		for (auto& cursor : cursors) {
			text += document.getLineText(cursor.getSelectionStart().line) + "\n";
		}
	}

	ImGui::SetClipboardText(text.c_str());
}


//
//	TextEditor::paste
//

void TextEditor::paste() {
	// ignore non-text clipboard content
	if (ImGui::GetClipboardText() != nullptr) {
		auto transaction = startTransaction();
		std::string clipboardText = ImGui::GetClipboardText();
		insertTextAtCursors(transaction, ImGui::GetClipboardText());
		endTransaction(transaction);
	}
}


//
//	TextEditor::undo
//

void TextEditor::undo() {
	if (transactions.canUndo()) {
		transactions.undo(document, cursors);
	}
}


//
//	TextEditor::redo
//

void TextEditor::redo() {
	if (transactions.canRedo()) {
		transactions.redo(document, cursors);
	}
}


//
//	TextEditor::selectFirstOccurrenceOf
//

void TextEditor::selectFirstOccurrenceOf(const std::string& text, bool caseSensitive, bool wholeWord) {
	Coordinate start, end;

	if (document.findText(Coordinate(0, 0), text, caseSensitive, wholeWord, start, end)) {
		cursors.setCursor(start, end);
		ensureCursorIsVisible = true;

	} else {
		cursors.clearAdditional(true);
	}
}


//
//	TextEditor::selectNextOccurrenceOf
//

void TextEditor::selectNextOccurrenceOf(const std::string& text, bool caseSensitive, bool wholeWord) {
	Coordinate start, end;

	if (document.findText(cursors.getCurrent().getSelectionEnd(), text, caseSensitive, wholeWord, start, end)) {
		cursors.setCursor(start, end);
		ensureCursorIsVisible = true;

	} else {
		cursors.clearAdditional(true);
	}
}


//
//	TextEditor::selectAllOccurrencesOf
//

void TextEditor::selectAllOccurrencesOf(const std::string& text, bool caseSensitive, bool wholeWord) {
	Coordinate start, end;

	if (document.findText(Coordinate(0, 0), text, caseSensitive, wholeWord, start, end)) {
		cursors.setCursor(start, end);
		bool done = false;

		while (!done) {
			Coordinate nextStart, nextEnd;
			document.findText(cursors.getCurrent().getSelectionEnd(), text, caseSensitive, wholeWord, nextStart, nextEnd);

			if (nextStart == start && nextEnd == end) {
				done = true;

			} else {
				cursors.addCursor(nextStart, nextEnd);
			}
		}

		ensureCursorIsVisible = true;

	} else {
		cursors.clearAdditional(true);
	}
}


//
//	TextEditor::addNextOccurrence
//

void TextEditor::addNextOccurrence() {

	auto cursor = cursors.getCurrent();
	auto text = document.getSectionText(cursor.getSelectionStart(), cursor.getSelectionEnd());
	Coordinate start, end;

	if (document.findText(cursor.getSelectionEnd(), text, true, false, start, end)) {
		cursors.addCursor(start, end);
	}
}


//
//	TextEditor::addNextOccurrences
//

void TextEditor::selectAllOccurrences() {
	auto cursor = cursors.getCurrent();
	auto text = document.getSectionText(cursor.getSelectionStart(), cursor.getSelectionEnd());
	selectAllOccurrencesOf(text, true, false);
}


//
//	TextEditor::replaceTextInCurrentCursor
//

void TextEditor::replaceTextInCurrentCursor(const std::string& text) {
	auto transaction = startTransaction();

	// first delete old text
	auto cursor = cursors.getCurrentAsIterator();
	auto start = cursor->getSelectionStart();
	auto end = cursor->getSelectionEnd();

	deleteText(transaction, start, end);
	cursors.adjustForDelete(cursor, start, end);

	// now insert new text
	Coordinate newEnd = insertText(transaction, start, text);
	cursor->update(newEnd, false);
	cursors.adjustForInsert(cursor, start, newEnd);

	endTransaction(transaction);
}


//
//	TextEditor::replaceTextInAllCursors
//

void TextEditor::replaceTextInAllCursors(const std::string& text) {
	auto transaction = startTransaction();
	insertTextAtCursors(transaction, text);
	endTransaction(transaction);
}


//
//	TextEditor::moveUp
//

void TextEditor::moveUp(int lines, bool select) {
	for (auto& cursor : cursors) {
		cursor.update(document.getUp(cursor.getInteractiveEnd(), lines), select);
	}

	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveDown
//

void TextEditor::moveDown(int lines, bool select) {
	for (auto& cursor : cursors) {
		cursor.update(document.getDown(cursor.getInteractiveEnd(), lines), select);
	}

	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveLeft
//

void TextEditor::moveLeft(bool select, bool wordMode) {
	for (auto& cursor : cursors) {
		cursor.update(document.getLeft(cursor.getInteractiveEnd(), wordMode), select);
	}

	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveRight
//

void TextEditor::moveRight(bool select, bool wordMode) {
	for (auto& cursor : cursors) {
		cursor.update(document.getRight(cursor.getInteractiveEnd(), wordMode), select);
	}

	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveToTop
//

void TextEditor::moveToTop(bool select) {
	cursors.clearAdditional();
	cursors.updateCursor(document.getTop(), select);
	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveToBottom
//

void TextEditor::moveToBottom(bool select) {
	cursors.clearAdditional();
	cursors.updateCursor(document.getBottom(), select);
	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveToStartOfLine
//

void TextEditor::moveToStartOfLine(bool select) {
	cursors.clearAdditional();
	cursors.updateCursor(document.getStartOfLine(cursors.getCurrent().getInteractiveEnd()), select);
	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveToEndOfLine
//

void TextEditor::moveToEndOfLine(bool select) {
	cursors.clearAdditional();
	cursors.updateCursor(document.getEndOfLine(cursors.getCurrent().getInteractiveEnd()), select);
	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveTo
//

void TextEditor::moveTo(Coordinate coordinate, bool select) {
	cursors.clearAdditional();
	cursors.updateCursor(coordinate, select);
	ensureCursorIsVisible = true;
}


//
//	TextEditor::makeCursorVisible
//

void TextEditor::makeCursorVisible() {
	// this should npt be called directly as it requires the correct ImGui context
	// it should only be called by the render function
	auto cursor = cursors.getCurrent().getInteractiveEnd();

	if (cursor.line <= firstVisibleLine) {
		float targetScroll = std::max(0.0f, (cursor.line - 0.5f) * glyphSize.y);

		if (targetScroll < ImGui::GetScrollY()) {
			ImGui::SetScrollY(targetScroll);
		}

	} else if (cursor.line >= lastVisibleLine) {
		float targetScroll = std::max(0.0f, (cursor.line + 1.5f) * glyphSize.y - visibleHeight);

		if (targetScroll > ImGui::GetScrollY()) {
			ImGui::SetScrollY(targetScroll);
		}
	}

	if (cursor.column <= firstVisibleColumn) {
		float targetScroll = std::max(0.0f, (cursor.column - 0.5f) * glyphSize.x);

		if (targetScroll < ImGui::GetScrollX()) {
			ImGui::SetScrollX(targetScroll);
		}

	} else if (cursor.column >= lastVisibleColumn) {
		float targetScroll = std::max(0.0f, (cursor.column + 0.5f) * glyphSize.x - visibleWidth);

		if (targetScroll > ImGui::GetScrollX()) {
			ImGui::SetScrollX(targetScroll);
		}
	}
}


//
//	TextEditor::handleCharacter
//

void TextEditor::handleCharacter(ImWchar codepoint) {
	auto transaction = startTransaction();

	auto opener = static_cast<char>(codepoint);
	auto isPaired = completePairedGlyphs && (opener == '{' || opener == '[' || opener == '(' || opener == '"' || opener == '\'');
	auto closer = opener == '{' ? '}' : (opener == '[' ? ']' : (opener == '(' ? ')' : opener));

	if (cursors.anyHasSelection() && isPaired) {
		// encapsulate the current selections with the requested pairs
		for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
			if (cursor->hasSelection()) {
				auto start = cursor->getSelectionStart();
				auto end = cursor->getSelectionEnd();

				// insert the opening glyph
				auto end1 = insertText(transaction, end, std::string(1, closer));
				cursors.adjustForInsert(cursor, start, end1);

				// insert the closing glyph
				auto end2 = insertText(transaction, start, std::string(1, opener));
				cursors.adjustForInsert(cursor, start, end2);

				// update old selection
				cursor->update(Coordinate(start.line, start.column + 1), Coordinate(end.line, end.column + 1));
			}
		}

	} else if (isPaired) {
		// insert the requested pair
		std::string pair(2, 0);
		pair[0] = opener;
		pair[1] = closer;

		for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
			auto start = cursor->getSelectionStart();
			auto end = insertText(transaction, start, pair);
			cursors.adjustForInsert(cursor, start, end);
			cursor->update(Coordinate(start.line, start.column + 1), false);
		}

	} else if (autoIndent && codepoint == '\n') {
		// handle auto indent case
		autoIndentAtCursors(transaction);

	} else {
		// just insert a regular character
		std::string utf8(4, 0);
		std::string text(utf8.begin(), CodePoint::put(utf8.begin(), codepoint));
		insertTextAtCursors(transaction, text);
	}

	endTransaction(transaction);
}


//
//	TextEditor::handleBackspace
//

void TextEditor::handleBackspace(bool wordMode) {
	auto transaction = startTransaction();

	// remove selections or characters to the left of the cursor
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto start = cursor->hasSelection() ? cursor->getSelectionStart() : document.getLeft(cursor->getSelectionStart(), wordMode);
		auto end = cursor->getSelectionEnd();
		deleteText(transaction, start, end);
		cursor->update(start, false);
		cursors.adjustForDelete(cursor, start, end);
	}

	endTransaction(transaction);
}

//
//	TextEditor::handleDelete
//

void TextEditor::handleDelete(bool wordMode) {
	auto transaction = startTransaction();

	// remove selections or characters to the right of the cursor
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto start = cursor->getSelectionStart();
		auto end = cursor->hasSelection() ? cursor->getSelectionEnd() : document.getRight(cursor->getSelectionEnd(), wordMode);
		deleteText(transaction, start, end);
		cursor->update(start, false);
		cursors.adjustForDelete(cursor, start, end);
	}

	endTransaction(transaction);
}


//
//	TextEditor::removeSelectedLines
//

void TextEditor::removeSelectedLines() {
	auto transaction = startTransaction();

	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto start = document.getStartOfLine(cursor->getSelectionStart());
		auto end = cursor->getSelectionEnd();
		end = (end.column == 0) ? end : document.getNextLine(end);
		deleteText(transaction, start, end);
		cursor->update(start, false);
		cursors.adjustForDelete(cursor, start, end);
	}

	endTransaction(transaction);
}


//
//	TextEditor::insertLineAbove
//

void TextEditor::insertLineAbove() {
	auto transaction = startTransaction();

	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto start = document.getStartOfLine(cursor->getSelectionStart());
		auto end = insertText(transaction, start, "\n");
		cursor->update(start, false);
		cursors.adjustForInsert(cursor, start, end);
	}

	endTransaction(transaction);
}


//
//	TextEditor::insertLineBelow
//

void TextEditor::insertLineBelow() {
	auto transaction = startTransaction();

	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto start = cursor->getSelectionEnd();
		start = (start.column == 0) ? start : document.getNextLine(start);
		auto end = insertText(transaction, start, "\n");
		cursor->update(start, false);
		cursors.adjustForInsert(cursor, start, end);
	}

	endTransaction(transaction);
}


//
//	TextEditor::indentLine
//

void TextEditor::indentLine() {
	auto transaction = startTransaction();

	// process all cursors
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		if (cursor->hasSelection()) {
			auto cursorStart = cursor->getSelectionStart();
			auto cursorEnd = cursor->getInteractiveEnd();

			// process all lines in this cursor
			for (auto line = cursorStart.line; line <= cursorEnd.line; line++) {
				if (Coordinate(line, 0) != cursorEnd && document[line].glyphs()) {
					auto insertStart = Coordinate(line, 0);
					auto insertEnd = insertText(transaction, insertStart, "\t");
					cursors.adjustForInsert(cursor, insertStart, insertEnd);
				}
			}

			auto tabSize = document.getTabSize();
			cursorStart.column += cursorStart.column ? tabSize : 0;
			cursorEnd.column += cursorEnd.column ? tabSize : 0;
			cursor->update(cursorStart, cursorEnd);
		}
	}

	endTransaction(transaction);
}


//
//	TextEditor::deindentLine
//

void TextEditor::deindentLine() {
	auto transaction = startTransaction();

	// process all cursors
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		if (cursor->hasSelection()) {
			auto cursorStart = cursor->getSelectionStart();
			auto cursorEnd = cursor->getInteractiveEnd();
			auto tabSize = document.getTabSize();

			for (auto line = cursorStart.line; line <= cursorEnd.line; line++) {
					// determine how many whitespace are available at the start with a max of 4 columns
					int column = 0;
					int index = 0;

					while (column < 4 && index < document[line].glyphs() && std::isblank(document[line][index].character)) {
						column += document[line][index].character == '\t' ? tabSize - (column % tabSize) : 1;
						index++;
					}

					// delete that whitespace if required
					Coordinate deleteStart{line, 0};
					Coordinate deleteEnd{line, document.getColumn(line, index)};

					if (deleteEnd != deleteStart) {
						deleteText(transaction, deleteStart, deleteEnd);
						cursors.adjustForDelete(cursor, deleteStart, deleteEnd);
					}
			}
		}
	}

	endTransaction(transaction);
}


//
//	Widget::moveUpLines
//

void TextEditor::moveUpLines() {
	// don't move up if first line is in cursors
	if (cursors[0].getSelectionStart().line != 0) {
		auto transaction = startTransaction();

		for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
			auto start = cursor->getSelectionStart();
			auto end = cursor->getSelectionEnd();

			// delete existing lines
			auto deleteStart = document.getStartOfLine(start);
			auto deleteEnd = (end.column == 0) ? end : document.getNextLine(end);;
			auto text = document.getSectionText(deleteStart, deleteEnd);
			deleteText(transaction, deleteStart, deleteEnd);
			cursors.adjustForDelete(cursor, deleteStart, deleteEnd);

			// insert text one line up
			auto insertStart = document.getUp(deleteStart);
			auto insertEnd = insertText(transaction, insertStart, text);
			cursors.adjustForInsert(cursor, insertStart, insertEnd);

			// update cursor
			cursor->update(start - Coordinate(1, 0), end - Coordinate(1, 0));
		}

		endTransaction(transaction);
	}
}


//
//	TextEditor::moveDownLines
//

void TextEditor::moveDownLines() {
	// don't move up if last line is in cursors
	if (!document.isLastLine(cursors[cursors.size() - 1].getSelectionStart().line)) {
		auto transaction = startTransaction();

		for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
			auto start = cursor->getSelectionStart();
			auto end = cursor->getSelectionEnd();

			// delete existing lines
			auto deleteStart = document.getStartOfLine(start);
			auto deleteEnd = (end.column == 0) ? end : document.getNextLine(end);;
			auto text = document.getSectionText(deleteStart, deleteEnd);
			deleteText(transaction, deleteStart, deleteEnd);
			cursors.adjustForDelete(cursor, deleteStart, deleteEnd);

			// insert text one line down
			auto insertStart = document.getDown(deleteStart);
			auto insertEnd = insertText(transaction, insertStart, text);
			cursors.adjustForInsert(cursor, insertStart, insertEnd);

			// update cursor
			cursor->update(start + Coordinate(1, 0), end + Coordinate(1, 0));
		}

		endTransaction(transaction);
	}
}


//
//	TextEditor::toggleComment
//

void TextEditor::toggleComment() {
}


//
//	TextEditor::startTransaction
//

std::shared_ptr<TextEditor::Transaction> TextEditor::startTransaction() {
	std::shared_ptr<Transaction> transaction = Transactions::create();
	transaction->setBeforeState(cursors);
	return transaction;
}


//
//	TextEditor::endTransaction
//

bool TextEditor::endTransaction(std::shared_ptr<Transaction> transaction) {
	if (transaction->actions() > 0) {
		cursors.update();
		transaction->setAfterState(cursors);
		transactions.add(transaction);
		return true;

	} else {
		return false;
	}
}


//
//	TextEditor::insertTextAtCursors
//

void TextEditor::insertTextAtCursors(std::shared_ptr<Transaction> transaction, const std::string& text) {
	// delete any selection content first
	deleteTextAtCursors(transaction);

	// insert the text
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto start = cursor->getSelectionStart();
		auto end = insertText(transaction, start, text);
		cursor->update(end, false);
		cursors.adjustForInsert(cursor, start, end);
	}
}


//
//	TextEditor::deleteTextAtCursors
//

void TextEditor::deleteTextAtCursors(std::shared_ptr<Transaction> transaction) {
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		if (cursor->hasSelection()) {
			auto start = cursor->getSelectionStart();
			auto end = cursor->getSelectionEnd();
			deleteText(transaction, start, end);
			cursors.adjustForDelete(cursor, start, end);
		}
	}
}


//
//	TextEditor::autoIndentAtCursors
//

void TextEditor::autoIndentAtCursors(std::shared_ptr<Transaction> transaction) {
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto start = cursor->getSelectionStart();

		// delete any selections
		if (cursor->hasSelection()) {
			auto end = cursor->getSelectionEnd();
			deleteText(transaction, start, end);
			cursors.adjustForDelete(cursor, start, end);
		}

		// get previous and next character
		auto index = document.getIndex(start);
		auto& line = document[start.line];
		ImWchar previousChar = index > 0 ? line[index - 1].character : 0;
		ImWchar nextChar = index < line.size() ? line[index].character : 0;

		// remove extra whitespaces if required
		if (CodePoint::isWhiteSpace(nextChar)) {
			while (index < line.size() && CodePoint::isWhiteSpace(line[index].character)) {
				index++;
			}

			auto end = Coordinate(start.line, document.getColumn(start.line, index));
			deleteText(transaction, start, end);
			cursors.adjustForDelete(cursor, start, end);
		}

		// determine whitespace at start of current line
		std::string whitespace;

		for (auto i = 0; i < line.size() && CodePoint::isWhiteSpace(line[i].character); i++) {
			whitespace += line[i].character;
		}

		// determine text to insert
		std::string insert = "\n" + whitespace;
		auto newCursorIndex = static_cast<int>(whitespace.size());

		// handle special cases
		if (previousChar == '[' || previousChar == '{') {
			// add to an existing block
			insert += "\t";
			newCursorIndex++;

			if ((previousChar == '[' && nextChar == ']') || (previousChar == '{' && nextChar == '}')) {
				// open a new block
				insert += "\n" + whitespace;
			}
		}

		// insert new text
		auto end = insertText(transaction, start, insert);
		cursors.adjustForInsert(cursor, start, end);

		// set new cursor location
		cursor->update(Coordinate(start.line + 1, document.getColumn(start.line + 1, newCursorIndex)), false);
	}
}


//
//	TextEditor::insertText
//

TextEditor::Coordinate TextEditor::insertText(std::shared_ptr<Transaction> transaction, Coordinate start, const std::string& text) {
	// update document, add transaction and return coordinate of end of insert
	// this function does not touch the cursors
	auto end = document.insertText(start, text);
	transaction->addInsert(start, end, text);
	return end;
}


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
//	TextEditor::stripTrailingWhitespaces
//

void TextEditor::stripTrailingWhitespaces() {
	auto transaction = startTransaction();

	// process all the lines
	for (int i = 0; i < document.lines(); i++) {
		auto& line = document[i];
		int size = line.glyphs();
		int whitespace = -1;
		bool done = false;

		// look for first non-whitespace glyph at the end of the line
		for (auto index = size - 1; !done && index >= 0; index--) {
			if (CodePoint::isWhiteSpace(line[index].character)) {
				whitespace = index;

			} else {
				done = true;
			}
		}

		// remove whitespaces (if required)
		if (whitespace >= 0) {
			auto start = Coordinate(i, document.getColumn(line, whitespace));
			auto end = Coordinate(i, document.getColumn(line, size));
			deleteText(transaction, start, end);
		}
	}

	// update cursor if transaction wasn't empty
	if (endTransaction(transaction)) {
		cursors.setCursor(document.normalizeCoordinate(cursors.getCurrent().getSelectionEnd()));
	}
}

//
//	TextEditor::filterLines
//

void TextEditor::filterLines(std::function<std::string(std::string)> filter) {
	auto transaction = startTransaction();

	// process all the lines
	for (int i = 0; i < document.lines(); i++) {
		// get original text and run it through filter
		auto before = document.getLineText(i);
		std::string after = filter(before);

		// update line if anything changed
		if (after != before) {
			auto start = Coordinate(i, 0);
			auto end = document.getEndOfLine(start);
			deleteText(transaction, start, end);
			insertText(transaction, start, after);
		}
	}

	// update cursor if transaction wasn't empty
	if (endTransaction(transaction)) {
		cursors.setCursor(document.normalizeCoordinate(cursors.getCurrent().getSelectionEnd()));
	}
}


//
//	TextEditor::tabsToSpaces
//

void TextEditor::tabsToSpaces() {
	filterLines([this](const std::string& input) {
		auto tabSize = document.getTabSize();
		std::string output;

		for (auto c : input) {
			if (c == '\t') {
				output.append(tabSize - (output.size() % tabSize), ' ');

			} else {
				output += c;
			}
		}

		return output;
	});
}


//
//	TextEditor::spacesToTabs
//

void TextEditor::spacesToTabs() {
	FilterLines([this](const std::string& input) {
		auto tabSize = document.getTabSize();
		std::string output;
		int pos = 0;
		int spaces = 0;

		for (auto c : input) {
			if (c == ' ') {
				spaces++;

			} else {
				while (spaces) {
					int spacesUntilNextTab = tabSize - (pos % tabSize);

					if (spacesUntilNextTab == 1) {
						output += ' ';
						pos++;
						spaces--;

					} else if (spaces >= spacesUntilNextTab) {
						output += '\t';
						pos += spacesUntilNextTab;
						spaces -= spacesUntilNextTab;

					} else if (c != '\t')
						while (spaces) {
							output += ' ';
							pos++;
							spaces--;
						}

					else {
						spaces = 0;
					}
				}

				if (c == '\t') {
					output += '\t';
					pos += tabSize - (pos % tabSize);

				} else {
					output += c;
					pos++;
				}
			}
		}

		return output;
	});
}


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

inline void TextEditor::Cursors::setCursor(Coordinate start, Coordinate end) {
	clear();
	emplace_back(start, end);
	current = 0;
}


//
//	TextEditor::Cursors::addCursor
//

inline void TextEditor::Cursors::addCursor(Coordinate start, Coordinate end) {
	at(current).setCurrent(false);
	emplace_back(start, end);
	current = size() - 1;
}


//
//	TextEditor::Cursors::anyHasSelection
//

inline bool TextEditor::Cursors::anyHasSelection() const {
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
	current = 0;
}


//
//	TextEditor::Cursors::clearAdditional
//

void TextEditor::Cursors::clearAdditional(bool reset) {
	for (auto cursor = begin(); cursor < end();) {
		if (!cursor->isCurrent()) {
			cursor = erase(cursor);

		} else {
			cursor++;
		}
	}

	current = 0;
	at(0).setCurrent(true);

	if (reset) {
		at(0).resetToStart();
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


//
//	TextEditor::Line::maxColumn
//

int TextEditor::Line::maxColumn(int tabSize) const {
	// determine the maximum column number for this line
	int column = 0;

	for (auto glyph = begin(); glyph < end(); glyph++) {
		column = (glyph->character == '\t') ? ((column / tabSize) + 1) * tabSize : column + 1;
	}

	return column;
}


//
//	TextEditor::Line::columnToIndex
//

int TextEditor::Line::columnToIndex(int column, int tabSize) const {
	// convert a column reference to a glyph index for this line (taking tabs into account)
	int index = 0;
	int c = 0;

	for (auto glyph = begin(); glyph < end(); glyph++) {
		if (c == column) {
			return index;

		} if (c > column) {
			return index - 1;
		}

		c = (glyph->character == '\t') ? ((c / tabSize) + 1) * tabSize : c + 1;
		index++;
	}

	return index;
}


//
//	TextEditor::Line::indexToColumn
//

int TextEditor::Line::indexToColumn(int index, int tabSize) const {
	// convert a glyph index to a column reference for this line (taking tabs into account)
	int column = 0;

	for (auto glyph = begin(); glyph < begin() + index; glyph++) {
		column = (glyph->character == '\t') ? ((column / tabSize) + 1) * tabSize : column + 1;
	}

	return column;
}


//
//	TextEditor::Document::setText
//

void TextEditor::Document::setText(const std::string& text) {
	// reset document
	clear();
	emplace_back();

	// process input UTF-8 and generate lines of glyphs
	auto i = text.begin();

	while (i < text.end()) {
		ImWchar codepoint;
		i = CodePoint::get(i, &codepoint);

		if (codepoint == '\n') {
			emplace_back();

		} else if (codepoint != '\r') {
			back().emplace_back(Glyph(codepoint, Color::standard));
		}
	}

	// colorize the text
	colorize();
}


//
//	TextEditor::Document::getText
//

std::string TextEditor::Document::getText() const {
	// process all glyphs and generate UTF-8 output
	std::string text;
	std::string utf8(4, 0);

	for (auto i = begin(); i < end(); i++) {
		for (auto& glyph : *i) {
			text.append(utf8.begin(), CodePoint::put(utf8.begin(), glyph.character));
		}

		text += "\n";
	}

	return text;
}


//
//	TextEditor::Document::getSectionText
//

std::string TextEditor::Document::getSectionText(Coordinate start, Coordinate end) const {
	std::string section;

	auto lineNo = start.line;
	auto index = getIndex(start);
	auto endIndex = getIndex(end);
	std::string utf8(4, 0);

	while (lineNo < end.line || index < endIndex) {
		auto& line = at(lineNo);

		if (index < line.glyphs()) {
			section.append(utf8.begin(), CodePoint::put(utf8.begin(), line[index].character));

			index++;
		} else {
			section += '\n';
			lineNo++;
			index = 0;
		}
	}

	return section;
}


//
//	TextEditor::Document::getLineText
//

std::string TextEditor::Document::getLineText(int line) const {
	return getSectionText(Coordinate(line, 0), Coordinate(line, maxColumn(line)));
}


//
//	TextEditor::Document::insertText
//

TextEditor::Coordinate TextEditor::Document::insertText(Coordinate start, const std::string& text) {
	auto line = begin() + start.line;
	auto index = getIndex(start);
	auto lineNo = start.line;

	// process input as UTF-8
	auto i = text.begin();

	// process all codepoints
	while (i < text.end()) {
		ImWchar codepoint;
		i = CodePoint::get(i, &codepoint);

		if (codepoint == '\n') {
			// split line
			insert(line + 1, Line());
			line = begin() + lineNo;
			auto nextLine = begin() + ++lineNo;

			for (auto i = line->begin() + index; i < line->end(); i++) {
				nextLine->push_back(*i);
			}

			line->erase(line->begin() + index, line->end());
			line = nextLine;
			index = 0;

		} else if (codepoint != '\r') {
			// insert next glyph
			line->insert(line->begin() + (index++), Glyph(codepoint, Color::standard));
		}
	}

	// determine end-of insert
	auto end = Coordinate(lineNo, getColumn(static_cast<int>(line - begin()), index));

	// colorize the new lines
	colorize(start.line, end.line);
	return end;
}


//
//	TextEditor::Document::deleteText
//

void TextEditor::Document::deleteText(Coordinate start, Coordinate end) {
	auto& startLine = at(start.line);
	auto& endLine = at(end.line);
	auto startIndex = getIndex(start);
	auto endIndex = getIndex(end);

	// see if start and end are on the same line
	if (start.line == end.line) {
		startLine.erase(startLine.begin() + startIndex, startLine.begin() + endIndex);

	// start and end are on different lines
	} else {
		// remove start of last line
		endLine.erase(endLine.begin(), endLine.begin() + endIndex);

		// remove full lines
		erase(begin() + start.line + 1, begin() + end.line);

		// remove end of first line
		auto& startLine = at(start.line);
		startLine.erase(startLine.begin() + startIndex, startLine.end());

		// join lines
		auto& nextLine = at(start.line + 1);
		startLine.insert(startLine.end(), nextLine.begin(), nextLine.end());
		erase(begin() + start.line + 1);
	}

	colorize(start.line, start.line + 1);
}


//
//	TextEditor::Document::maxColumn
//

int TextEditor::Document::maxColumn() const {
	// determine the maximum column number for this document
	int result = 0;

	for (auto& line : *this) {
		result = std::max(result, maxColumn(line));
	}

	return result;
}


//
//	TextEditor::Document::getUp
//

TextEditor::Coordinate TextEditor::Document::getUp(Coordinate from, int lines) const {
	return normalizeCoordinate(Coordinate(from.line - lines, from.column));
}


//
//	TextEditor::Document::getDown
//

TextEditor::Coordinate TextEditor::Document::getDown(Coordinate from, int lines) const {
	return normalizeCoordinate(Coordinate(from.line + lines, from.column));
}


//
//	TextEditor::Document::getLeft
//

TextEditor::Coordinate TextEditor::Document::getLeft(Coordinate from, bool wordMode) const {
	if (wordMode) {
		if (from.column == 0) {
			if (from.line) {
				from = Coordinate(from.line - 1, maxColumn(from.line - 1));
			}
		}

		return findWordStart(from);

	} else {
		auto index = getIndex(from);

		if (index == 0) {
			return (from.line > 0) ? Coordinate(from.line - 1, maxColumn(from.line - 1)) : from;

		} else {
			return Coordinate(from.line, getColumn(from.line, index - 1));
		}
	}
}


//
//	TextEditor::Document::getRight
//

TextEditor::Coordinate TextEditor::Document::getRight(Coordinate from, bool wordMode) const {
	if (wordMode) {
		if (isEndOfLine(from)) {
			if (from.line < lines() - 1) {
				from = Coordinate(from.line + 1, 0);
			}
		}

		return findWordEnd(from);

	} else {
		auto index = getIndex(from);

		if (index == at(from.line).glyphs()) {
			return (from.line < lines() - 1) ? Coordinate(from.line + 1, 0) : from;

		} else {
			return Coordinate(from.line, getColumn(from.line, index + 1));
		}
	}
}


//
//	TextEditor::Document::getTop
//

TextEditor::Coordinate TextEditor::Document::getTop() const {
	return Coordinate(0, 0);
}


//
//	TextEditor::Document::getBottom
//

TextEditor::Coordinate TextEditor::Document::getBottom() const {
	auto lastLine = lines() - 1;
	return Coordinate(lastLine, maxColumn(lastLine));
}


//
//	TextEditor::Document::getStartOfLine
//

TextEditor::Coordinate TextEditor::Document::getStartOfLine(Coordinate from) const {
	return Coordinate(from.line, 0);
}


//
//	TextEditor::Document::getEndOfLine
//

TextEditor::Coordinate TextEditor::Document::getEndOfLine(Coordinate from) const {
	return Coordinate(from.line, maxColumn(from.line));
}


//
//	TextEditor::Document::findWordStart
//

TextEditor::Coordinate TextEditor::Document::findWordStart(Coordinate from) const {
	if (from.column == 0) {
		return from;

	} else {
		auto& line = at(from.line);
		auto index = getIndex(from);
		auto firstCharacter = line[index - 1].character;

		if (CodePoint::isWhiteSpace(firstCharacter)) {
			while (index > 0 && CodePoint::isWhiteSpace(line[index - 1].character)) {
				index--;
			}

		} else if (CodePoint::isAlphaNumeric(firstCharacter)) {
			while (index > 0 && CodePoint::isAlphaNumeric(line[index - 1].character)) {
				index--;
			}

		} else if (index > 0) {
			index--;
		}

		return Coordinate(from.line, getColumn(line, index));
	}
}


//
//	TextEditor::Document::findWordEnd
//

TextEditor::Coordinate TextEditor::Document::findWordEnd(Coordinate from) const {
	auto& line = at(from.line);
	auto index = getIndex(from);
	auto size = line.glyphs();

	auto firstCharacter = line[index].character;

	if (CodePoint::isWhiteSpace(firstCharacter)) {
		while (index < size && CodePoint::isWhiteSpace(line[index].character)) {
			index++;
		}

	} else if (CodePoint::isAlphaNumeric(firstCharacter)) {
		while (index < size && CodePoint::isAlphaNumeric(line[index].character)) {
			index++;
		}

	} else if (index < size) {
		index++;
	}

	return Coordinate(from.line, getColumn(line, index));
}


//
//	TextEditor::Document::findText
//

bool TextEditor::Document::findText(Coordinate from, const std::string& text, bool caseSensitive, bool wholeWord, Coordinate& start, Coordinate& end) const {
	// convert input string to vector of codepoints
	std::vector<ImWchar> search;
	auto i = text.begin();

	while (i < text.end()) {
		ImWchar codepoint;
		i = CodePoint::get(i, &codepoint);
		search.emplace_back(caseSensitive ? codepoint : CodePoint::toLower(codepoint));
	}

	// search document
	auto startLine = from.line;
	auto startIndex = getIndex(from);
	auto searchLine = startLine;
	auto searchIndex = startIndex;

	do {
		auto line = searchLine;
		auto index = searchIndex;
		auto lineSize = at(line).glyphs();
		bool done = false;
		int i = 0;

		while (!done && i < search.size()) {
			if (search[i] == '\n') {
				if (index == lineSize) {
					if (line == lines() - 1) {
						done = true;

					} else {
						line++;
						index = 0;
						lineSize = at(line).glyphs();
						i++;
					}

				} else {
					done = true;
				}

			} else {
				if (index == lineSize) {
					done = true;

				} else {
					auto ch = at(line)[index].character;

					if (!caseSensitive) {
						ch = CodePoint::toLower(ch);
					}

					if (ch == search[i]) {
						index++;
						i++;

					} else {
						done = true;
					}
				}
			}
		}

		if (i == search.size()) {
			start = Coordinate(searchLine, getColumn(searchLine, searchIndex));
			end = Coordinate(line, getColumn(line, index));

			if (!wholeWord || isWholeWord(start, end)) {
				return true;
			}
		}

		if (searchIndex == at(searchLine).glyphs()) {
			searchLine = (searchLine == lines() - 1) ? 0 : searchLine + 1;
			searchIndex = 0;

		} else {
			searchIndex++;
		}

	} while (searchLine != startLine || searchIndex != startIndex);

	return false;
}


//
//	TextEditor::Document::isWholeWord
//

bool TextEditor::Document::isWholeWord(Coordinate start, Coordinate end) const {
	if (start.line != end.line || end.column - start.column < 1) {
		return false;

	} else {
		auto wordStart = findWordStart(Coordinate(start.line, start.column + 1));
		auto wordEnd = findWordEnd(Coordinate(end.line, end.column - 1));
		return start == wordStart && end == wordEnd;
	}
}


//
//	TextEditor::Document::normalizeCoordinate
//

TextEditor::Coordinate TextEditor::Document::normalizeCoordinate(Coordinate coordinate) const {
	Coordinate result;

	if (coordinate.line < 0) {
		result = Coordinate(0, 0);

	} else if (coordinate.line >= lines()) {
		result = Coordinate(lines() - 1, maxColumn(lines() - 1));

	} else {
		result = Coordinate(coordinate.line, std::max(0, std::min(coordinate.column, maxColumn(coordinate.line))));
	}

	return Coordinate(result.line, getColumn(result.line, getIndex(result)));
}


//
//	TextEditor::Document::setLanguage
//

void TextEditor::Document::setLanguage(const Language &definition) {
	language = &definition;
	colorize();
}


//
//	TextEditor::Document::colorize
//

TextEditor::State TextEditor::Document::colorize(Line& line) {
	auto state = line.state;

	// process all glyphs in line
	auto glyph = line.begin();

	while (glyph < line.end()) {
		if (state == State::inText) {
			// mark whitespace characters
			if (CodePoint::isWhiteSpace(glyph->character)) {
				(glyph++)->color = Color::whitespace;

			// handle single line comments
			} else if (language->singleLineComment1.size() && line.matches(glyph, language->singleLineComment1)) {
				line.setColor(glyph, line.end(), Color::comment);
				glyph = line.end();

			} else if (language->singleLineComment2.size() && line.matches(glyph, language->singleLineComment2)) {
				line.setColor(glyph, line.end(), Color::comment);
				glyph = line.end();

			// are we starting a multiline comment
			} else if (language->commentStart.size() && line.matches(glyph, language->commentStart)) {
				state = State::inComment;
				auto size = language->commentEnd.size();
				line.setColor(glyph, glyph + size, Color::comment);
				glyph += size;

			// are we starting a single quoted string
			} else if (language->hasSingleQuoteStrings && glyph->character == '\'') {
				state = State::inSingleQuoteString;
				(glyph++)->color = Color::string;

			// are we starting a double quoted string
			} else if (language->hasDoubleQuoteStrings && glyph->character == '"') {
				state = State::inDoubleQuoteString;
				(glyph++)->color = Color::string;

			// is this a preprocessor line
			} else if (language->preprocess && glyph == line.begin() && glyph->character == language->preprocess) {
				line.setColor(line.begin(), line.end(), Color::preprocessor);
				glyph = line.end();

			} else {
				(glyph++)->color = Color::standard;
			}

		} else if (state == State::inComment) {
			// stay in comment state until we see the end sequemce
			if (line.matches(glyph, language->commentEnd)) {
				auto size = language->commentEnd.size();
				line.setColor(glyph, glyph + size, Color::comment);
				glyph += size;
				state = State::inText;

			} else {
				(glyph++)->color = Color::comment;
			}

		} else if (state == State::inSingleQuoteString) {
			// stay in single quote state until we see an end
			if (glyph->character == language->stringEscapeCharacter) {
				// skip escaped character
				(glyph++)->color = Color::string;

				if (glyph < line.end()) {
					(glyph++)->color = Color::string;
				}

			} else if (glyph->character == '\'') {
				(glyph++)->color = Color::string;
				state = State::inText;

			} else {
				(glyph++)->color = Color::string;
			}

		} else if (state == State::inDoubleQuoteString) {
			// stay in double quote state until we see an end
			if (glyph->character == language->stringEscapeCharacter) {
				// skip escaped character
				(glyph++)->color = Color::string;

				if (glyph < line.end()) {
					(glyph++)->color = Color::string;
				}

			} else if (glyph->character == '"') {
				(glyph++)->color = Color::string;
				state = State::inText;

			} else {
				(glyph++)->color = Color::string;
			}
		}
	}

	return state;
}

void TextEditor::Document::colorize(int start, int end) {
	if (language) {
		for (auto line = start; line <= end; line++) {
			State state = colorize(at(line));

			if (line < lines() - 1) {
				auto& nextLine = at(line + 1);

				if (nextLine.state != state) {
					at(line + 1).state = state;

					if (line == end) {
						end++;
					}
				}
			}
		}
	}
}

void TextEditor::Document::colorize() {
	if (language) {
		colorize(0, lines() - 1);
	}
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


//
//	TextEditor::Iterator::operator*
//

TextEditor::Iterator::reference TextEditor::Iterator::operator*() const {
	return static_cast<Line*>(line)->at(index).character;
}


//
//	TextEditor::Iterator::operator->
//

TextEditor::Iterator::pointer TextEditor::Iterator::operator->() const {
	return &(static_cast<Line*>(line)->at(index).character);
}


//
//	TextEditor::updatePalette
//

void TextEditor::updatePalette() {
	// Update palette with the current alpha from style
	paletteAlpha = ImGui::GetStyle().Alpha;

	for (int i = 0; i < static_cast<size_t>(Color::count); i++) {
		auto color = ImGui::ColorConvertU32ToFloat4(paletteBase[i]);
		color.w *= paletteAlpha;
		palette[i] = ImGui::ColorConvertFloat4ToU32(color);
	}
}


//
//	Color palettes
//

const TextEditor::Palette& TextEditor::GetDarkPalette() {
	const static Palette p = {{
		IM_COL32(220, 223, 228, 255),	// standard
		IM_COL32(224, 108, 117, 255),	// keyword
		IM_COL32(229, 192, 123, 255),	// number
		IM_COL32(152, 195, 121, 255),	// string
		IM_COL32(106, 115, 132, 255),	// punctuation
		IM_COL32(128, 128,  64, 255),	// preprocessor
		IM_COL32(220, 223, 228, 255),	// identifier
		IM_COL32( 97, 175, 239, 255),	// known identifier
		IM_COL32( 54, 150, 162, 255),	// comment
		IM_COL32( 40,  44,  52, 255),	// background
		IM_COL32(224, 224, 224, 255),	// cursor
		IM_COL32( 32,  96, 160, 128),	// selection
		IM_COL32(255,  32,   0, 128),	// errorMarker
		IM_COL32(255, 255, 255,  21),	// whitespace
		IM_COL32(122, 131, 148, 255),	// line number
		IM_COL32(255, 255, 255, 255),	// current line number
	}};

	return p;
}

const TextEditor::Palette& TextEditor::GetLightPalette()
{
	const static Palette p = {{
		IM_COL32( 64,  64,  64, 255),	// standard
		IM_COL32(  6,  12, 255, 255),	// keyword
		IM_COL32(  0, 128,   0, 255),	// number
		IM_COL32(160,  32,  32, 255),	// string
		IM_COL32(  0,   0,   0, 255),	// punctuation
		IM_COL32( 96,  96,  64, 255),	// preprocessor
		IM_COL32( 64,  64,  64, 255),	// identifier
		IM_COL32( 16,  96,  96, 255),	// known identifier
		IM_COL32( 32,  80,  32, 255),	// comment
		IM_COL32(255, 255, 255, 255),	// background
		IM_COL32(  0,   0,   0, 255),	// cursor
		IM_COL32(  0,   0,  96,  64),	// selection
		IM_COL32(255,  16,   0, 160),	// errorMarker
		IM_COL32(144, 144, 144, 144),	// whitespace
		IM_COL32(  0,  80,  80, 255),	// line number
		IM_COL32(  0,   0,   0, 255),	// current line number
	}};

	return p;
}

const TextEditor::Palette& TextEditor::GetRetroBluePalette() {
	const static Palette p = {{
		IM_COL32(255, 255,   0, 255),	// standard
		IM_COL32(  0, 255, 255, 255),	// keyword
		IM_COL32(  0, 255,   0, 255),	// number
		IM_COL32(  0, 128, 128, 255),	// string
		IM_COL32(255, 255, 255, 255),	// punctuation
		IM_COL32(  0, 128,   0, 255),	// preprocessor
		IM_COL32(255, 255,   0, 255),	// identifier
		IM_COL32(255, 255, 255, 255),	// known identifier
		IM_COL32(128, 128, 128, 255),	// comment
		IM_COL32(  0,   0, 128, 255),	// background
		IM_COL32(255, 128,   0, 255),	// cursor
		IM_COL32(  0, 255, 255, 128),	// selection
		IM_COL32(255,   0,   0, 160),	// errorMarker
		IM_COL32(255, 255, 255,  80),	// whitespace
		IM_COL32(  0, 128, 128, 255),	// line number
		IM_COL32(255, 255, 255, 255),	// current line number
	}};

	return p;
}

const TextEditor::Palette& TextEditor::GetMarianaPalette() {
	const static Palette p = {{
		IM_COL32(255, 255, 255, 255),	// standard
		IM_COL32(198, 149, 198, 255),	// keyword
		IM_COL32(249, 174,  88, 255),	// number
		IM_COL32(153, 199, 148, 255),	// string
		IM_COL32( 95, 180, 180, 255),	// punctuation
		IM_COL32(128, 128,  64, 255),	// preprocessor
		IM_COL32(255, 255, 255, 255),	// identifier
		IM_COL32(224, 160, 255, 255),	// preprocessor
		IM_COL32(166, 172, 185, 255),	// comment
		IM_COL32( 48,  56,  65, 255),	// background
		IM_COL32(224, 224, 224, 255),	// cursor
		IM_COL32(110, 122, 133, 128),	// selection
		IM_COL32(236,  95, 102, 128),	// errorMarker
		IM_COL32(255, 255, 255,  48),	// whitespace
		IM_COL32(255, 255, 255, 176),	// line number
		IM_COL32(255, 255, 255, 255),	// current line number
	}};

	return p;
}

TextEditor::Palette TextEditor::defaultPalette = TextEditor::GetDarkPalette();
