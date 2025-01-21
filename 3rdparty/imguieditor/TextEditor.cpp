//	TextEditor - A syntax highlighting text editor for ImGui
//	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

#include "TextEditor.h"


//
//	TextEditor::setText
//

void TextEditor::setText(const std::string &text) {
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

	// adjust text start in case line numbers are shown
	if (showLineNumbers) {
		int digits = static_cast<int>(std::log10(document.lines() + 1) + 1.0f);
		textStart += glyphSize.x * (digits + lineNumberMargin);
	}

	// set style
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertU32ToFloat4(palette.get(Color::background)));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	// start a new child window
	// this must be done before we we handle keyboard and mouse interactions to ensure correct ImGui context
	int longestLine = document.maxColumn();
	ImGui::SetNextWindowContentSize({textStart + longestLine * glyphSize.x + cursorWidth, document.lines() * glyphSize.y});
	ImGui::BeginChild(title, size, border, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoNavInputs);

	// handle keyboard and mouse inputs
	handleKeyboardInputs();
	handleMouseInteractions();

	// ensure cursors are up to date (sorted and merged if required)
	if (cursors.anyHasUpdate()) {
		cursors.update();
	}

	// recolorize document if showMatchingBrackets option has changed
	if (lastShowMatchingBrackets != showMatchingBrackets) {
		document.colorize();
	}

	// rebuild bracket list (if document has changes or showMatchingBrackets option has changed)
	if (showMatchingBrackets && (!lastShowMatchingBrackets || document.isUpdated())) {
		brackets.update(document);
	}

	lastShowMatchingBrackets = showMatchingBrackets;

	// ensure cursor is visible (if requested)
	if (ensureCursorIsVisible) {
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

		ensureCursorIsVisible = false;
	}

	// determine view parameters
	float scrollbarSize = ImGui::GetStyle().ScrollbarSize;
	visibleHeight = ImGui::GetWindowHeight() - ((longestLine >= visibleColumns) ? scrollbarSize : 0.0f);
	visibleLines = std::max(static_cast<int>(std::ceil(visibleHeight / glyphSize.y)), 0);
	firstVisibleLine = std::max(static_cast<int>(std::floor(ImGui::GetScrollY() / glyphSize.y)), 0);
	lastVisibleLine = std::min(static_cast<int>(std::ceil((ImGui::GetScrollY() + visibleHeight) / glyphSize.y)), document.lines() - 1);

	auto tabSize = document.getTabSize();
	visibleWidth = ImGui::GetWindowWidth() - textStart - ((document.lines() >= visibleLines) ? scrollbarSize : 0.0f);
	visibleColumns = std::max(static_cast<int>(std::ceil(visibleWidth / glyphSize.x)), 0);
	firstVisibleColumn = (std::max(static_cast<int>(std::floor(ImGui::GetScrollX() / glyphSize.x)), 0) / tabSize) * tabSize;
	lastVisibleColumn = ((static_cast<int>(std::ceil((ImGui::GetScrollX() + visibleWidth) / glyphSize.x)) + 1) / tabSize) * tabSize;

	// render editor parts
	renderSelections();
	renderErrorMarkers();
	renderMatchingBrackets();
	renderText();
	renderCursors();
	renderLineNumbers();

	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}


//
//	TextEditor::renderSelections
//

void TextEditor::renderSelections() {
	auto drawList = ImGui::GetWindowDrawList();
	ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();

	// draw background for selections
	for (auto& cursor : cursors) {
		if (cursor.hasSelection()) {
			auto start = cursor.getSelectionStart();
			auto end = cursor.getSelectionEnd();

			if (end.line >= firstVisibleLine && start.line <= lastVisibleLine) {
				auto first = std::max(start.line, firstVisibleLine);
				auto last = std::min(end.line, lastVisibleLine);

				for (auto line = first; line <= last; line++) {
					auto x = cursorScreenPos.x + textStart;
					auto left = x + (line == first ? start.column : 0) * glyphSize.x;
					auto right = x + (line == last ? end.column : document.maxColumn(line)) * glyphSize.x;
					auto y = cursorScreenPos.y + line * glyphSize.y;
					drawList->AddRectFilled(ImVec2(left, y), ImVec2(right, y + glyphSize.y), palette.get(Color::selection));
				}
			}
		}
	}
}


//
//	TextEditor::renderErrorMarkers
//

void TextEditor::renderErrorMarkers() {
	if (errorMarkers.size()) {
		auto drawList = ImGui::GetWindowDrawList();
		ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();

		for (int line = firstVisibleLine; line <= lastVisibleLine; line++) {
			if (document[line].errorMarker) {
				auto left = cursorScreenPos.x + textStart;
				auto right = left + document.maxColumn(line) * glyphSize.x;
				auto y = cursorScreenPos.y + line * glyphSize.y;
				auto start = ImVec2(left, y);
				auto end = ImVec2(right, y + glyphSize.y);
				drawList->AddRectFilled(start, end, palette.get(Color::errorMarker));

				if (ImGui::IsMouseHoveringRect(start, end)) {
					ImGui::PushStyleColor(ImGuiCol_PopupBg, palette.get(Color::errorMarker));
					ImGui::BeginTooltip();
					ImGui::TextUnformatted(errorMarkers[document[line].errorMarker - 1].c_str());
					ImGui::EndTooltip();
					ImGui::PopStyleColor();
				}
			}
		}
	}
}


//
//	TextEditor::renderMatchingBrackets
//

void TextEditor::renderMatchingBrackets() {
	if (showMatchingBrackets) {
		if (brackets.size()) {
			auto drawList = ImGui::GetWindowDrawList();
			ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();

			// render bracket pair lines
			for (auto& bracket : brackets) {
				if ((bracket.end.line - bracket.start.line) > 1 &&
					bracket.start.line <= lastVisibleLine &&
					bracket.end.line > firstVisibleLine) {

					auto lineX = cursorScreenPos.x + textStart + std::min(bracket.start.column, bracket.end.column) * glyphSize.x;
					auto startY = cursorScreenPos.y + (bracket.start.line + 1) * glyphSize.y;
					auto endY = cursorScreenPos.y + bracket.end.line * glyphSize.y;
					drawList->AddLine(ImVec2(lineX, startY), ImVec2(lineX, endY), palette.get(Color::whitespace), 1.0f);
				}
			}

			// render active bracket pair
			auto active = brackets.getActive(cursors.getMain().getInteractiveEnd());

			if (active != brackets.end() &&
				active->start.line <= lastVisibleLine &&
				active->end.line > firstVisibleLine) {

				auto x1 = cursorScreenPos.x + textStart + active->start.column * glyphSize.x;
				auto y1 = cursorScreenPos.y + active->start.line * glyphSize.y;
				drawList->AddRectFilled(ImVec2(x1, y1), ImVec2(x1 + glyphSize.x, y1 + glyphSize.y), palette.get(Color::whitespace));

				auto x2 = cursorScreenPos.x + textStart + active->end.column * glyphSize.x;
				auto y2 = cursorScreenPos.y + active->end.line * glyphSize.y;
				drawList->AddRectFilled(ImVec2(x2, y2), ImVec2(x2 + glyphSize.x, y2 + glyphSize.y), palette.get(Color::whitespace));

				if (active->end.line - active->start.line > 1) {
					auto lineX = std::min(x1, x2);
					drawList->AddLine(ImVec2(lineX, y1 + glyphSize.y), ImVec2(lineX, y2), palette.get(Color::cursor), 1.0f);
				}
			}
		}
	}
}


//
//	TextEditor::renderText
//

void TextEditor::renderText() {
	auto drawList = ImGui::GetWindowDrawList();
	ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
	ImVec2 lineScreenPos = cursorScreenPos + ImVec2(textStart, firstVisibleLine * glyphSize.y);
	auto tabSize = document.getTabSize();

	for (int i = firstVisibleLine; i <= lastVisibleLine; i++) {
		auto& line = document[i];

		// draw colored glyphs for current line
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

					drawList->AddLine(p1, p2, palette.get(Color::whitespace));
					drawList->AddLine(p2, p3, palette.get(Color::whitespace));
					drawList->AddLine(p2, p4, palette.get(Color::whitespace));
				}

			} else if (character == ' ') {
				if (showWhitespaces) {
					const auto x = glyphPos.x + glyphSize.x * 0.5f;
					const auto y = glyphPos.y + fontSize * 0.5f;
					drawList->AddCircleFilled(ImVec2(x, y), 1.5f, palette.get(Color::whitespace), 4);
				}

			} else {
				font->RenderChar(drawList, fontSize, glyphPos, palette.get(glyph.color), character);
			}

			index++;
			column += (character == '\t') ? tabSize - (column % tabSize) : 1;
		}

		lineScreenPos.y += glyphSize.y;
	}
}


//
//	TextEditor::renderCursors
//

void TextEditor::renderCursors() {
	// update cursor animation timer
	cursorAnimationTimer = std::fmod(cursorAnimationTimer + ImGui::GetIO().DeltaTime, 1.0f);

	if (ImGui::IsWindowFocused() && (!ImGui::GetIO().ConfigInputTextCursorBlink || cursorAnimationTimer < 0.5f)) {
		auto drawList = ImGui::GetWindowDrawList();
		ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();

		for (auto& cursor : cursors) {
			auto pos = cursor.getInteractiveEnd();

			if (pos.line >= firstVisibleLine && pos.line <= lastVisibleLine) {
				auto x = cursorScreenPos.x + textStart + pos.column * glyphSize.x;
				auto y = cursorScreenPos.y + pos.line * glyphSize.y;
				drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + cursorWidth, y + glyphSize.y), palette.get(Color::cursor));
			}
		}
	}
}


//
//	TextEditor::renderLineNumbers
//

void TextEditor::renderLineNumbers() {
	if (showLineNumbers) {
		auto drawList = ImGui::GetWindowDrawList();
		ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
		auto lineNumberEnd = ImGui::GetWindowPos().x + textStart;

		// erase background in case we are scrolling horizontally
		if (ImGui::GetScrollX() > 0.0f) {
			drawList->AddRectFilled(
				ImGui::GetWindowPos(),
				ImGui::GetWindowPos() + ImVec2(textStart,
				ImGui::GetWindowSize().y),
				palette.get(Color::background));
		}

		// get the line number of the last cursor
		auto curserLine = cursors.getCurrent().getInteractiveEnd().line;

		for (int i = firstVisibleLine; i <= lastVisibleLine; i++) {
			int digits = static_cast<int>(std::log10(i + 1) + 1.0f);
			auto lineNumberWidth = (digits + lineNumberMargin) * glyphSize.x;
			Color foreground =  i == curserLine ? Color::currentLineNumber : Color::lineNumber;

			drawList->AddText(
				ImVec2(lineNumberEnd - lineNumberWidth,
				cursorScreenPos.y + i * glyphSize.y),
				palette.get(foreground),
				std::to_string(i + 1).c_str());
		}
	}
}


//
//	TextEditor::handleKeyboardInputs
//

void TextEditor::handleKeyboardInputs() {
	if (ImGui::IsWindowFocused()) {
		ImGuiIO& io = ImGui::GetIO();
		io.WantCaptureKeyboard = true;
		io.WantTextInput = true;

		// get state of modifier keys
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
		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_LeftBracket)) { deindentLines(); }
		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_RightBracket)) { indentLines(); }
		else if (!readOnly && isAltOnly && ImGui::IsKeyPressed(ImGuiKey_UpArrow)) { moveUpLines(); }
		else if (!readOnly && isAltOnly && ImGui::IsKeyPressed(ImGuiKey_DownArrow)) { moveDownLines(); }
		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_Slash) && document.hasLanguage()) { toggleComments(); }

		// change insert mode
		else if (isNoModifiers && ImGui::IsKeyPressed(ImGuiKey_Insert)) { overwrite = !overwrite; }

		// handle new line
		else if (!readOnly && isNoModifiers && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))) { handleCharacter('\n'); }
		else if (!readOnly && isShortcut && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))) { insertLineBelow(); }
		else if (!readOnly && isShiftShortcut && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))) { insertLineAbove(); }

		// handle tabs
		else if (!readOnly && isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_Tab)) {
			if (cursors.anyHasSelection()) {
				if (shift) {
					deindentLines();

				} else {
					indentLines();
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
			std::max(0, static_cast<int>(std::floor(mousePos.y / glyphSize.y))),
			std::max(0, static_cast<int>(std::floor((mousePos.x - textStart) / glyphSize.x)))
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
				cursor.update(cursor.getInteractiveEnd() < cursor.getInteractiveStart() ? start : end);

			} else {
				cursors.updateCurrentCursor(mouseCoord);
			}

			ensureCursorIsVisible = true;

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
					cursors.updateCurrentCursor(start, end);
				}

			} else if (doubleClick) {
				// left mouse button double click
				if (!overLineNumbers) {
					auto start = document.findWordStart(mouseCoord);
					auto end = document.findWordEnd(mouseCoord);
					cursors.updateCurrentCursor(start, end);
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
						cursor.update(cursor.getInteractiveEnd() < cursor.getInteractiveStart() ? start : end);

					} else if (addCursor) {
						cursors.addCursor(start, end);

					} else {
						cursors.setCursor(start, end);
					}

				// handle mouse clicks over text
				} else {
					if (extendCursor) {
						cursors.updateCurrentCursor(mouseCoord);

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
	// copy selections to clipboard and remove them
	copy();
	auto transaction = startTransaction();
	deleteTextFromAllCursors(transaction);
	cursors.getCurrent().resetToStart();
	endTransaction(transaction);
}


//
//	TextEditor::copy
//

void TextEditor::copy() const {
	// copy all selections and put them on the clipboard
	// empty cursors copy the entire line
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
	auto clipboard = ImGui::GetClipboardText();

	if (clipboard) {
		auto transaction = startTransaction();
		insertTextIntoAllCursors(transaction, clipboard);
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
	insertTextIntoAllCursors(transaction, text);
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
	cursors.updateCurrentCursor(document.getTop(), select);
	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveToBottom
//

void TextEditor::moveToBottom(bool select) {
	cursors.clearAdditional();
	cursors.updateCurrentCursor(document.getBottom(), select);
	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveToStartOfLine
//

void TextEditor::moveToStartOfLine(bool select) {
	cursors.clearAdditional();
	cursors.updateCurrentCursor(document.getStartOfLine(cursors.getCurrent().getInteractiveEnd()), select);
	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveToEndOfLine
//

void TextEditor::moveToEndOfLine(bool select) {
	cursors.clearAdditional();
	cursors.updateCurrentCursor(document.getEndOfLine(cursors.getCurrent().getInteractiveEnd()), select);
	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveTo
//

void TextEditor::moveTo(Coordinate coordinate, bool select) {
	cursors.clearAdditional();
	cursors.updateCurrentCursor(coordinate, select);
	ensureCursorIsVisible = true;
}


//
//	TextEditor::handleCharacter
//

void TextEditor::handleCharacter(ImWchar character) {
	auto transaction = startTransaction();

	auto opener = static_cast<char>(character);
	auto isPaired = !overwrite && completePairedGlyphs && (opener == '{' || opener == '[' || opener == '(' || opener == '"' || opener == '\'');
	auto closer = opener == '{' ? '}' : (opener == '[' ? ']' : (opener == '(' ? ')' : opener));

	// ignore input if it was the closing character for a pair that was automatically inserted
	if (completePairCloser) {
		if (completePairCloser == character && completePairLocation == cursors.getCurrent().getSelectionEnd()) {
			completePairCloser = 0;
			return;
		}

		completePairCloser = 0;
	}

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

		// remember the closer
		completePairCloser = closer;
		completePairLocation = cursors.getCurrent().getSelectionEnd();

	} else if (!overwrite && autoIndent && character == '\n') {
		// handle auto indent case
		autoIndentAllCursors(transaction);

	} else {
		// handle overwrite by deleting next glyph before insert
		if (overwrite) {
			for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
				if (!cursor->hasSelection()) {
					auto start = cursor->getSelectionStart();

					if (start != document.getEndOfLine(start)) {
						auto end = document.getRight(start);
						deleteText(transaction, start, end);
						cursors.adjustForDelete(cursor, start, end);
					}
				}
			}
		}

		// just insert a regular character
		std::string utf8(4, 0);
		std::string text(utf8.begin(), CodePoint::write(utf8.begin(), character));
		insertTextIntoAllCursors(transaction, text);
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
//	TextEditor::indentLines
//

void TextEditor::indentLines() {
	auto transaction = startTransaction();

	// process all cursors
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto cursorStart = cursor->getSelectionStart();
		auto cursorEnd = cursor->getSelectionEnd();

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

	endTransaction(transaction);
}


//
//	TextEditor::deindentLines
//

void TextEditor::deindentLines() {
	auto transaction = startTransaction();

	// process all cursors
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto cursorStart = cursor->getSelectionStart();
		auto cursorEnd = cursor->getSelectionEnd();
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

	endTransaction(transaction);
}


//
//	Widget::moveUpLines
//

void TextEditor::moveUpLines() {
	// don't move up if first line is in one of the cursors
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
	// don't move up if last line is in one of the cursors
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
//	TextEditor::toggleComments
//

void TextEditor::toggleComments() {
	auto transaction = startTransaction();
	auto comment = document.getLanguage().singleLineComment;

	// process all cursors
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto cursorStart = cursor->getSelectionStart();
		auto cursorEnd = cursor->getSelectionEnd();

		// process all lines in this cursor
		for (auto line = cursorStart.line; line <= cursorEnd.line; line++) {
			if (Coordinate(line, 0) != cursorEnd && document[line].glyphs()) {
				// see if line starts with a comment (after possible leading whitespaces)
				int start = 0;
				int i = 0;

				while (start < document[line].glyphs() && CodePoint::isWhiteSpace(document[line][start].character)) {
					start++;
				}

				while (start + i < document[line].glyphs() && i < comment.size() && document[line][start + i].character == comment[i]) {
					i++;
				}

				if (i == comment.size()) {
					auto deleteStart = Coordinate(line, document.getColumn(line, start));
					auto deleteEnd = Coordinate(line, document.getColumn(line, start + static_cast<int>(comment.size()) + 1));
					deleteText(transaction, deleteStart, deleteEnd);
					cursors.adjustForDelete(cursor, deleteStart, deleteEnd);

				} else {
					auto insertStart = Coordinate(line, document.getColumn(line, start));
					auto insertEnd = insertText(transaction, insertStart, comment + " ");
					cursors.adjustForInsert(cursor, insertStart, insertEnd);
				}
			}
		}
	}

	endTransaction(transaction);
}


//
//	TextEditor::filterSelections
//

void TextEditor::filterSelections(std::function<std::string(std::string)> filter) {
	auto transaction = startTransaction();

	// process all cursors
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto start = cursor->getSelectionStart();
		auto end = cursor->getSelectionEnd();

		// process all lines in this cursor
		for (auto line = start.line; line <= end.line; line++) {
			if (Coordinate(line, 0) != end && document[line].glyphs()) {
				// get original text and run it through filter
				auto before = document.getSectionText(start, end);
				std::string after = filter(before);

				// update selection if anything changed
				if (after != before) {
					deleteText(transaction, start, end);
					cursors.adjustForDelete(cursor, start, end);
					auto newEnd = insertText(transaction, start, after);
					cursor->update(start, newEnd);
					cursors.adjustForInsert(cursor, start, newEnd);
				}
			}
		}
	}

	endTransaction(transaction);
}


//
//	TextEditor::selectionToLowerCase
//

void TextEditor::selectionToLowerCase() {
	FilterSelections([](const std::string& text) {
		std::string result;
		auto end = text.end();
		auto i = text.begin();
		std::string utf8(4, 0);

		while (i < end) {
			ImWchar codepoint;
			i = CodePoint::read(i, end, &codepoint);
			result.append(utf8.begin(), CodePoint::write(utf8.begin(), CodePoint::toLower(codepoint)));
		}

		return result;
	});
}


//
//	TextEditor::selectionToUpperCase
//

void TextEditor::selectionToUpperCase() {
	FilterSelections([](const std::string& text) {
		std::string result;
		auto end = text.end();
		auto i = text.begin();
		std::string utf8(4, 0);

		while (i < end) {
			ImWchar codepoint;
			i = CodePoint::read(i, end, &codepoint);
			result.append(utf8.begin(), CodePoint::write(utf8.begin(), CodePoint::toUpper(codepoint)));
		}

		return result;
	});
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
//	TextEditor::insertTextIntoAllCursors
//

void TextEditor::insertTextIntoAllCursors(std::shared_ptr<Transaction> transaction, const std::string& text) {
	// delete any selection content first
	deleteTextFromAllCursors(transaction);

	// insert the text
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto start = cursor->getSelectionStart();
		auto end = insertText(transaction, start, text);
		cursor->update(end, false);
		cursors.adjustForInsert(cursor, start, end);
	}
}


//
//	TextEditor::deleteTextFromAllCursors
//

void TextEditor::deleteTextFromAllCursors(std::shared_ptr<Transaction> transaction) {
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
//	TextEditor::autoIndentAllCursors
//

void TextEditor::autoIndentAllCursors(std::shared_ptr<Transaction> transaction) {
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
//	TextEditor::AddErrorMarker
//

void TextEditor::AddErrorMarker(int line, const std::string &marker) {
	if (line >= 0 && line < document.lines()) {
		errorMarkers.emplace_back(marker);
		document[line].errorMarker = errorMarkers.size();
	}
}


//
//	TextEditor::ClearErrorMarkers
//

void TextEditor::ClearErrorMarkers() {
	for (auto& line : document) {
		line.errorMarker = 0;
	}

	errorMarkers.clear();
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
		IM_COL32(224, 224, 224, 255),	// text
		IM_COL32(197, 134, 192, 255),	// keyword
		IM_COL32( 90, 179, 155, 255),	// declaration
		IM_COL32(181, 206, 168, 255),	// number
		IM_COL32(206, 145, 120, 255),	// string
		IM_COL32(255, 255, 153, 255),	// punctuation
		IM_COL32( 64, 192, 128, 255),	// preprocessor
		IM_COL32(156, 220, 254, 255),	// identifier
		IM_COL32( 79, 193, 255, 255),	// known identifier
		IM_COL32(106, 153,  85, 255),	// comment
		IM_COL32( 30,  30,  30, 255),	// background
		IM_COL32(224, 224, 224, 255),	// cursor
		IM_COL32( 32,  96, 160, 255),	// selection
		IM_COL32(128,   0,  32, 255),	// errorMarker
		IM_COL32( 90,  90,  90, 255),	// whitespace
		IM_COL32(246, 222,  36, 255),	// matchingBracketLevel1
		IM_COL32( 66, 120, 198, 255),	// matchingBracketLevel2
		IM_COL32(213,  96, 213, 255),	// matchingBracketLevel3
		IM_COL32(198,   8,  32, 255),	// matchingBracketError
		IM_COL32(128, 128, 144, 255),	// line number
		IM_COL32(224, 224, 240, 255),	// current line number
	}};

	return p;
}

const TextEditor::Palette& TextEditor::GetLightPalette()
{
	const static Palette p = {{
		IM_COL32( 64,  64,  64, 255),	// text
		IM_COL32(  6,  12, 255, 255),	// keyword
		IM_COL32(  6,  12, 255, 255),	// declaration
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
		IM_COL32(246, 222,  36, 255),	// matchingBracketLevel1
		IM_COL32( 66, 120, 198, 255),	// matchingBracketLevel2
		IM_COL32(213,  96, 213, 255),	// matchingBracketLevel3
		IM_COL32(198,   8,  32, 255),	// matchingBracketError
		IM_COL32(  0,  80,  80, 255),	// line number
		IM_COL32(  0,   0,   0, 255),	// current line number
	}};

	return p;
}

TextEditor::Palette TextEditor::defaultPalette = TextEditor::GetDarkPalette();
