//	TextEditor - A syntax highlighting text editor for Dear ImGui.
//	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>
#include <limits>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "imgui.h"
#include "imgui_internal.h"

#include "TextEditor.h"


//
//	TextEditor::setText
//

void TextEditor::setText(const std::string_view &text) {
	// load text into document and reset subsystems
	document.setText(text);
	transactions.reset();
	bracketeer.reset();
	cursors.clearAll();
	clearMarkers();
	makeCursorVisible();
}


//
//	TextEditor::render
//

void TextEditor::render(const char* title, const ImVec2& size, bool border) {
	// update color palette (if required)
	if (paletteAlpha != ImGui::GetStyle().Alpha) {
		updatePalette();
	}

	// get font information and determine horizontal offsets for line numbers, decorations and text
	font = ImGui::GetFont();
	fontSize = ImGui::GetFontSize();
	glyphSize = ImVec2(font->CalcTextSizeA(fontSize, FLT_MAX, -1.0f, "#").x, ImGui::GetTextLineHeightWithSpacing() * lineSpacing);
	lineNumberLeftOffset = leftMargin * glyphSize.x;

	if (showLineNumbers) {
		int digits = static_cast<int>(std::log10(document.lineCount() + 1) + 1.0f);
		lineNumberRightOffset = lineNumberLeftOffset + digits * glyphSize.x;
		decorationOffset = lineNumberRightOffset + decorationMargin * glyphSize.x;

	} else {
		lineNumberRightOffset = lineNumberLeftOffset;
		decorationOffset = lineNumberLeftOffset;
	}

	if (decoratorWidth > 0.0f) {
		textOffset = decorationOffset + decoratorWidth + decorationMargin * glyphSize.x;

	} else if (decoratorWidth < 0.0f) {
		textOffset = decorationOffset + (-decoratorWidth + decorationMargin) * glyphSize.x;

	} else {
		textOffset = decorationOffset + textMargin * glyphSize.x;
	}

	// get current position and total/visible editor size
	auto pos = ImGui::GetCursorScreenPos();
	auto totalSize = ImVec2(textOffset + document.getMaxColumn() * glyphSize.x + cursorWidth, document.size() * glyphSize.y);
	auto region = ImGui::GetContentRegionAvail();
	auto visibleSize = ImGui::CalcItemSize(size, region.x, region.y); // messing with Dear ImGui internals

	// see if we have scrollbars
	float scrollbarSize = ImGui::GetStyle().ScrollbarSize;
	verticalScrollBarSize = (totalSize.y > visibleSize.y) ? scrollbarSize : 0.0f;
	horizontalScrollBarSize = (totalSize.x > visibleSize.x) ? scrollbarSize : 0.0f;

	// determine visible lines and columns
	visibleWidth = visibleSize.x - textOffset - verticalScrollBarSize;
	visibleColumns = std::max(static_cast<int>(std::ceil(visibleWidth / glyphSize.x)), 0);
	visibleHeight = visibleSize.y - horizontalScrollBarSize;
	visibleLines = std::max(static_cast<int>(std::ceil(visibleHeight / glyphSize.y)), 0);

	// determine scrolling requirements
	float scrollX = -1.0f;
	float scrollY = -1.0f;

	// ensure cursor is visible (if requested)
	if (ensureCursorIsVisible) {
		auto cursor = cursors.getCurrent().getInteractiveEnd();

		if (cursor.line <= firstVisibleLine + 1) {
			scrollY = std::max(0.0f, (cursor.line - 2.0f) * glyphSize.y);

		} else if (cursor.line >= lastVisibleLine - 1) {
			scrollY = std::max(0.0f, (cursor.line + 2.0f) * glyphSize.y - visibleHeight);
		}

		if (cursor.column <= firstVisibleColumn + 1) {
			scrollX = std::max(0.0f, (cursor.column - 2.0f) * glyphSize.x);

		} else if (cursor.column >= lastVisibleColumn - 1) {
			scrollX = std::max(0.0f, (cursor.column + 2.0f) * glyphSize.x - visibleWidth);
		}

		ensureCursorIsVisible = false;
	}

	// scroll to specified line (if required)
	if (scrollToLineNumber >= 0) {
		scrollToLineNumber = std::min(scrollToLineNumber, document.lineCount());
		scrollX = 0.0f;

		switch (scrollToAlignment) {
			case Scroll::alignTop:
				scrollY = std::max(0.0f, static_cast<float>(scrollToLineNumber) * glyphSize.y);
				break;

			case Scroll::alignMiddle:
				scrollY = std::max(0.0f, static_cast<float>(scrollToLineNumber - visibleLines / 2) * glyphSize.y);
				break;

			case Scroll::alignBottom:
				scrollY = std::max(0.0f, static_cast<float>(scrollToLineNumber - (visibleLines - 1)) * glyphSize.y);
				break;
		}

		scrollToLineNumber = -1;
	}

	// set scroll (if required)
	if (scrollX >= 0.0f || scrollY >= 0.0f) {
		ImGui::SetNextWindowScroll(ImVec2(scrollX, scrollY));
	}

	// ensure editor has focus (if required)
	if (focusOnEditor) {
		ImGui::SetNextWindowFocus();
		focusOnEditor = false;
	}

	// start a new child window
	// this must be done before we handle keyboard and mouse interactions to ensure correct Dear ImGui context
	ImGui::SetNextWindowContentSize(totalSize);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertU32ToFloat4(palette.get(Color::background)));
	ImGui::BeginChild(title, size, border, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoNavInputs);

	// handle keyboard and mouse inputs
	handleKeyboardInputs();
	handleMouseInteractions();

	// ensure cursors are up to date (sorted and merged if required)
	if (cursors.anyHasUpdate()) {
		cursors.update();
	}

	// recolorize entire document and reset brackets (if required)
	if (showMatchingBracketsChanged || languageChanged) {
		colorizer.updateEntireDocument(document, language);
		bracketeer.reset();
	}

	// was document changed during this frame?
	auto documentChanged = document.isUpdated();

	if (language) {
		if (documentChanged) {
			// recolorize updated lines
			colorizer.updateChangedLines(document, language);
		}

		if (showMatchingBrackets && (documentChanged || showMatchingBracketsChanged || languageChanged)) {
			// rebuild bracket list
			bracketeer.update(document);
		}
	}

	// reset changed states
	showMatchingBracketsChanged = false;
	languageChanged = false;

	// determine view parameters
	firstVisibleColumn = std::max(static_cast<int>(std::floor(ImGui::GetScrollX() / glyphSize.x)), 0);
	lastVisibleColumn = static_cast<int>(std::floor((ImGui::GetScrollX() + visibleWidth) / glyphSize.x));
	firstVisibleLine = std::max(static_cast<int>(std::floor(ImGui::GetScrollY() / glyphSize.y)), 0);
	lastVisibleLine = std::min(static_cast<int>(std::floor((ImGui::GetScrollY() + visibleHeight) / glyphSize.y)), document.lineCount() - 1);

	// render editor parts
	renderSelections();
	renderMarkers();
	renderMatchingBrackets();
	renderText();
	renderCursors();
	renderMargin();
	renderLineNumbers();
	renderDecorations();
	renderScrollbarMiniMap();
	renderPanScrollIndicator();

	if (ImGui::BeginPopup("LineNumberContextMenu")) {
		lineNumberContextMenuCallback(contextMenuLine);
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("TextContextMenu")) {
		textContextMenuCallback(contextMenuLine, contextMenuColumn);
		ImGui::EndPopup();
	}

	// render find/replace popup
	renderFindReplace(pos, visibleSize.x - verticalScrollBarSize);

	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
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
					auto x = cursorScreenPos.x + textOffset;
					auto left = x + (line == first ? start.column : 0) * glyphSize.x;
					auto right = x + (line == last ? end.column : document[line].maxColumn) * glyphSize.x;
					auto y = cursorScreenPos.y + line * glyphSize.y;
					drawList->AddRectFilled(ImVec2(left, y), ImVec2(right, y + glyphSize.y), palette.get(Color::selection));
				}
			}
		}
	}
}


//
//	TextEditor::renderMarkers
//

void TextEditor::renderMarkers() {
	if (markers.size()) {
		auto drawList = ImGui::GetWindowDrawList();
		ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();

		for (int line = firstVisibleLine; line <= lastVisibleLine; line++) {
			if (document[line].marker) {
				auto& marker = markers[document[line].marker - 1];
				auto y = cursorScreenPos.y + line * glyphSize.y;

				if (((marker.lineNumberColor >> IM_COL32_A_SHIFT) & 0xFF) != 0) {
					auto left = cursorScreenPos.x + lineNumberLeftOffset;
					auto right = cursorScreenPos.x + lineNumberRightOffset;
					auto start = ImVec2(left, y);
					auto end = ImVec2(right, y + glyphSize.y);
					drawList->AddRectFilled(start, end, marker.lineNumberColor);

					if (marker.lineNumberTooltip.size() && ImGui::IsMouseHoveringRect(start, end)) {
						ImGui::PushStyleColor(ImGuiCol_PopupBg, marker.lineNumberColor);
						ImGui::BeginTooltip();
						ImGui::TextUnformatted(marker.lineNumberTooltip.c_str());
						ImGui::EndTooltip();
						ImGui::PopStyleColor();
					}
				}

				if (((marker.textColor >> IM_COL32_A_SHIFT) & 0xFF) != 0) {
					auto left = cursorScreenPos.x + textOffset;
					auto right = left + lastVisibleColumn * glyphSize.x;
					auto start = ImVec2(left, y);
					auto end = ImVec2(right, y + glyphSize.y);
					drawList->AddRectFilled(start, end, marker.textColor);

					if (marker.textTooltip.size() && ImGui::IsMouseHoveringRect(start, end)) {
						ImGui::PushStyleColor(ImGuiCol_PopupBg, marker.textColor);
						ImGui::BeginTooltip();
						ImGui::TextUnformatted(marker.textTooltip.c_str());
						ImGui::EndTooltip();
						ImGui::PopStyleColor();
					}
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
		if (bracketeer.size()) {
			auto drawList = ImGui::GetWindowDrawList();
			ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();

			// render bracket pair lines
			for (auto& bracket : bracketeer) {
				if ((bracket.end.line - bracket.start.line) > 1 &&
					bracket.start.line <= lastVisibleLine &&
					bracket.end.line > firstVisibleLine) {

					auto lineX = cursorScreenPos.x + textOffset + std::min(bracket.start.column, bracket.end.column) * glyphSize.x;
					auto startY = cursorScreenPos.y + (bracket.start.line + 1) * glyphSize.y;
					auto endY = cursorScreenPos.y + bracket.end.line * glyphSize.y;
					drawList->AddLine(ImVec2(lineX, startY), ImVec2(lineX, endY), palette.get(Color::whitespace), 1.0f);
				}
			}

			// render active bracket pair
			auto active = bracketeer.getEnclosingBrackets(cursors.getMain().getInteractiveEnd());

			if (active != bracketeer.end() &&
				active->start.line <= lastVisibleLine &&
				active->end.line > firstVisibleLine) {

				auto x1 = cursorScreenPos.x + textOffset + active->start.column * glyphSize.x;
				auto y1 = cursorScreenPos.y + active->start.line * glyphSize.y;
				drawList->AddRectFilled(ImVec2(x1, y1), ImVec2(x1 + glyphSize.x, y1 + glyphSize.y), palette.get(Color::matchingBracketBackground));

				auto x2 = cursorScreenPos.x + textOffset + active->end.column * glyphSize.x;
				auto y2 = cursorScreenPos.y + active->end.line * glyphSize.y;
				drawList->AddRectFilled(ImVec2(x2, y2), ImVec2(x2 + glyphSize.x, y2 + glyphSize.y), palette.get(Color::matchingBracketBackground));

				if (active->end.line - active->start.line > 1) {
					auto lineX = std::min(x1, x2);
					drawList->AddLine(ImVec2(lineX, y1 + glyphSize.y), ImVec2(lineX, y2), palette.get(Color::matchingBracketActive), 1.0f);
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
	ImVec2 lineScreenPos = cursorScreenPos + ImVec2(textOffset, firstVisibleLine * glyphSize.y);
	auto tabSize = document.getTabSize();
	auto firstRenderableColumn = (firstVisibleColumn / tabSize) * tabSize;

	for (int i = firstVisibleLine; i <= lastVisibleLine; i++) {
		auto& line = document[i];

		// draw colored glyphs for current line
		auto column = firstRenderableColumn;
		auto index = document.getIndex(line, column);
		auto lineSize = line.size();

		while (index < lineSize && column <= lastVisibleColumn) {
			auto& glyph = line[index];
			auto codepoint = glyph.codepoint;
			ImVec2 glyphPos{lineScreenPos.x + column * glyphSize.x, lineScreenPos.y};

			if (codepoint == '\t') {
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

			} else if (codepoint == ' ') {
				if (showWhitespaces) {
					const auto x = glyphPos.x + glyphSize.x * 0.5f;
					const auto y = glyphPos.y + fontSize * 0.5f;
					drawList->AddCircleFilled(ImVec2(x, y), 1.5f, palette.get(Color::whitespace), 4);
				}

			} else {
				font->RenderChar(drawList, fontSize, glyphPos, palette.get(glyph.color), codepoint);
			}

			index++;
			column += (codepoint == '\t') ? tabSize - (column % tabSize) : 1;
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

	if (ImGui::IsWindowFocused()) {
		ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();

		if (!ImGui::GetIO().ConfigInputTextCursorBlink || cursorAnimationTimer < 0.5f) {
			auto drawList = ImGui::GetWindowDrawList();

			for (auto& cursor : cursors) {
				auto pos = cursor.getInteractiveEnd();

				if (pos.line >= firstVisibleLine && pos.line <= lastVisibleLine) {
					auto x = cursorScreenPos.x + textOffset + pos.column * glyphSize.x - 1;
					auto y = cursorScreenPos.y + pos.line * glyphSize.y;
					drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + cursorWidth, y + glyphSize.y), palette.get(Color::cursor));
				}
			}
		}

		// notify OS of text input position for advanced Input Method Editor (IME)
		// this is required for the SDL3 backend as it will not report text input events unless we do this
		// see https://github.com/ocornut/imgui/issues/8584 for details
		if (!readOnly) {
			auto context = ImGui::GetCurrentContext();
			context->PlatformImeData.WantVisible = true;
			context->PlatformImeData.WantTextInput = true;
			context->PlatformImeData.InputPos = ImVec2(cursorScreenPos.x - 1.0f, cursorScreenPos.y - context->FontSize);
			context->PlatformImeData.InputLineHeight = context->FontSize;
			context->PlatformImeData.ViewportId = ImGui::GetCurrentWindow()->Viewport->ID;
		}
	}
}


//
//	TextEditor::renderMargin
//

void TextEditor::renderMargin() {
	if ((decoratorWidth != 0.0f && decoratorCallback) || showLineNumbers) {
		// erase background in case we are scrolling horizontally
		if (ImGui::GetScrollX() > 0.0f) {
			ImGui::GetWindowDrawList()->AddRectFilled(
				ImGui::GetWindowPos(),
				ImGui::GetWindowPos() + ImVec2(textOffset, ImGui::GetWindowSize().y),
				palette.get(Color::background));
		}
	}
}


//
//	TextEditor::renderLineNumbers
//

void TextEditor::renderLineNumbers() {
	if (showLineNumbers) {
		auto drawList = ImGui::GetWindowDrawList();
		auto cursorScreenPos = ImGui::GetCursorScreenPos();
		auto curserLine = cursors.getCurrent().getInteractiveEnd().line;
		auto position = ImVec2(ImGui::GetWindowPos().x + lineNumberRightOffset, cursorScreenPos.y);

		for (int i = firstVisibleLine; i <= lastVisibleLine; i++) {
			auto width = static_cast<int>(std::log10(i + 1) + 1.0f) * glyphSize.x;
			auto foreground = (i == curserLine) ? Color::currentLineNumber : Color::lineNumber;
			auto number = std::to_string(i + 1);
			drawList->AddText(position + ImVec2(-width, i * glyphSize.y), palette.get(foreground), number.c_str());
		}
	}
}


//
//	TextEditor::renderDecorations
//

void TextEditor::renderDecorations() {
	if (decoratorWidth != 0.0f && decoratorCallback) {
		auto cursorScreenPos = ImGui::GetCursorScreenPos();
		auto position = ImVec2(ImGui::GetWindowPos().x + decorationOffset, cursorScreenPos.y + glyphSize.y * firstVisibleLine);
		auto widthInPixels = (decoratorWidth < 0.0f) ? -decoratorWidth * glyphSize.x: decoratorWidth;
		Decorator decorator{0, widthInPixels, glyphSize.y, glyphSize};

		for (int i = firstVisibleLine; i <= lastVisibleLine; i++) {
			decorator.line = i;
			ImGui::SetCursorScreenPos(position);
			ImGui::PushID(i);
			decoratorCallback(decorator);
			ImGui::PopID();
			position.y += glyphSize.y;
		}

		ImGui::SetCursorScreenPos(cursorScreenPos);
	}
}


//
//	TextEditor::renderScrollbarMiniMap
//

void TextEditor::renderScrollbarMiniMap() {
	// based on https://github.com/ocornut/imgui/issues/3114
	// messing with Dear ImGui internals
	if (showScrollbarMiniMap) {
		auto window = ImGui::GetCurrentWindow();

		if (window->ScrollbarY) {
			auto drawList = ImGui::GetWindowDrawList();
			auto rect = ImGui::GetWindowScrollbarRect(window, ImGuiAxis_Y);
			auto lineHeight = rect.GetHeight() / static_cast<float>(document.size());
			auto offset = (rect.Max.x - rect.Min.x) * 0.3f;
			auto left = rect.Min.x + offset;
			auto right = rect.Max.x - offset;

			drawList->PushClipRect(rect.Min, rect.Max, false);

			// render cursor locations
			for (auto& cursor : cursors) {
				auto begin = cursor.getSelectionStart();
				auto end = cursor.getSelectionEnd();

				auto ly1 = std::round(rect.Min.y + begin.line * lineHeight);
				auto ly2 = std::round(rect.Min.y + (end.line + 1) * lineHeight);

				drawList->AddRectFilled(ImVec2(left, ly1), ImVec2(right, ly2), palette.get(Color::selection));
			}

			// render marker locations
			if (markers.size()) {
				for (size_t line = 0; line < document.size(); line++) {
					if (document[line].marker) {
						auto color = markers[document[line].marker - 1].textColor;

						if (!color) {
							color = markers[document[line].marker - 1].lineNumberColor;
						}

						auto ly = std::round(rect.Min.y + line * lineHeight);
						drawList->AddRectFilled(ImVec2(left, ly), ImVec2(right, ly + lineHeight), color);
					}
				}
			}

			drawList->PopClipRect();
		}
	}
}


//
//	TextEditor::renderPanScrollIndicator
//

void TextEditor::renderPanScrollIndicator() {
	if (showPanScrollIndicator && (panning || scrolling)) {
		auto drawList = ImGui::GetWindowDrawList();
		auto center =ImGui::GetWindowPos() + ImGui::GetWindowSize() / 2.0f;
		static constexpr int alpha = 160;
		drawList->AddCircleFilled(center, 20.0f, IM_COL32(255, 255, 255, alpha));
		drawList->AddCircle(center, 5.0f, IM_COL32(0, 0, 0, alpha), 0, 2.0f);

		drawList->AddTriangle(
			ImVec2(center.x - 15.0f, center.y),
			ImVec2(center.x - 8.0f, center.y - 4.0f),
			ImVec2(center.x - 8.0f, center.y + 4.0f),
			IM_COL32(0, 0, 0, alpha),
			2.0f);

		drawList->AddTriangle(
			ImVec2(center.x + 15.0f, center.y),
			ImVec2(center.x + 8.0f, center.y - 4.0f),
			ImVec2(center.x + 8.0f, center.y + 4.0f),
			IM_COL32(0, 0, 0, alpha),
			2.0f);

		drawList->AddTriangle(
			ImVec2(center.x, center.y - 15.0f),
			ImVec2(center.x - 4.0f, center.y - 8.0f),
			ImVec2(center.x + 4.0f, center.y - 8.0f),
			IM_COL32(0, 0, 0, alpha),
			2.0f);

		drawList->AddTriangle(
			ImVec2(center.x, center.y + 15.0f),
			ImVec2(center.x - 4.0f, center.y + 8.0f),
			ImVec2(center.x + 4.0f, center.y + 8.0f),
			IM_COL32(0, 0, 0, alpha),
			2.0f);
	}
}


//
//	latchButton
//

static bool latchButton(const char* label, bool* value, const ImVec2& size) {
	bool changed = false;
	ImVec4* colors = ImGui::GetStyle().Colors;

	if (*value) {
		ImGui::PushStyleColor(ImGuiCol_Button, colors[ImGuiCol_ButtonActive]);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors[ImGuiCol_ButtonActive]);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors[ImGuiCol_TableBorderLight]);

	} else {
		ImGui::PushStyleColor(ImGuiCol_Button, colors[ImGuiCol_TableBorderLight]);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors[ImGuiCol_TableBorderLight]);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors[ImGuiCol_ButtonActive]);
	}

	ImGui::Button(label, size);

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
		*value = !*value;
		changed = true;
	}

	ImGui::PopStyleColor(3);
	return changed;
}


//
//	inputString
//

static bool inputString(const char* label, std::string* value, ImGuiInputTextFlags flags=ImGuiInputTextFlags_None) {
	flags |=
		ImGuiInputTextFlags_NoUndoRedo |
		ImGuiInputTextFlags_CallbackResize;

	return ImGui::InputText(label, (char*) value->c_str(), value->capacity() + 1, flags, [](ImGuiInputTextCallbackData* data) {
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
			std::string* value = (std::string*) data->UserData;
			value->resize(data->BufTextLen);
			data->Buf = (char*) value->c_str();
		}

		return 0;
	}, value);
}


//
//	TextEditor::renderFindReplace
//

void TextEditor::renderFindReplace(ImVec2 pos, float width) {
	// render find/replace window (if required)
	if (findReplaceVisible) {
		// save current screen position
		auto currentScreenPosition = ImGui::GetCursorScreenPos();

		// calculate sizes
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6.0f, 4.0f));
		auto& style = ImGui::GetStyle();
		auto fieldWidth = 250.0f;

		auto button1Width = ImGui::CalcTextSize(findButtonLabel.c_str()).x + style.ItemSpacing.x * 2.0f;
		auto button2Width = ImGui::CalcTextSize(findAllButtonLabel.c_str()).x + style.ItemSpacing.x * 2.0f;
		auto optionWidth = ImGui::CalcTextSize("Aa").x + style.ItemSpacing.x * 2.0f;

		if (!readOnly) {
			button1Width = std::max(button1Width, ImGui::CalcTextSize(replaceButtonLabel.c_str()).x + style.ItemSpacing.x * 2.0f);
			button2Width = std::max(button2Width, ImGui::CalcTextSize(replaceAllButtonLabel.c_str()).x + style.ItemSpacing.x * 2.0f);
		}

		auto windowHeight =
			style.ChildBorderSize * 2.0f +
			style.WindowPadding.y * 2.0f +
			ImGui::GetFrameHeight() +
			(readOnly ? 0.0f : (style.ItemSpacing.y + ImGui::GetFrameHeight()));

		auto windowWidth =
			style.ChildBorderSize * 2.0f +
			style.WindowPadding.x * 2.0f +
			fieldWidth + style.ItemSpacing.x +
			button1Width + style.ItemSpacing.x +
			button2Width + style.ItemSpacing.x +
			optionWidth * 3.0f + style.ItemSpacing.x * 2.0f;

		// create window
		ImGui::SetNextWindowPos(ImVec2(
			pos.x + width - windowWidth - style.ItemSpacing.x,
			pos.y + style.ItemSpacing.y * 2.0f));

		ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
		ImGui::SetNextWindowBgAlpha(0.75f);

		ImGui::BeginChild("find-replace", ImVec2(windowWidth, windowHeight), ImGuiChildFlags_Borders);
		ImGui::SetNextItemWidth(fieldWidth);

		if (focusOnFind) {
			ImGui::SetKeyboardFocusHere();
			focusOnFind = false;
		}

		if (inputString("###find", &findText, ImGuiInputTextFlags_AutoSelectAll)) {
			if (findText.size()) {
				selectFirstOccurrenceOf(findText, caseSensitiveFind, wholeWordFind);

			} else {
				cursors.clearAll();
			}
		}

		if (ImGui::IsItemDeactivated() && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))){
			focusOnEditor = true;
		}

		bool disableFindButtons = !findText.size();

		if (disableFindButtons) {
			ImGui::BeginDisabled();
		}

		ImGui::SameLine();

		if (ImGui::Button(findButtonLabel.c_str(), ImVec2(button1Width, 0.0f))) {
			find();
		}

		ImGui::SameLine();

		if (ImGui::Button(findAllButtonLabel.c_str(), ImVec2(button2Width, 0.0f))) {
			findAll();
		}

		if (disableFindButtons) {
			ImGui::EndDisabled();
		}

		ImGui::SameLine();

		if (latchButton("Aa", &caseSensitiveFind, ImVec2(optionWidth, 0.0f))) {
			find();
		}

		ImGui::SameLine();

		if (latchButton("[]", &wholeWordFind, ImVec2(optionWidth, 0.0f))) {
			find();
		}

		ImGui::SameLine();

		if (ImGui::Button("x", ImVec2(optionWidth, 0.0f))) {
			findReplaceVisible = false;
			focusOnEditor = true;
		}

		if (!readOnly) {
			ImGui::SetNextItemWidth(fieldWidth);
			inputString("###replace", &replaceText);
			ImGui::SameLine();

			bool disableReplaceButtons = !findText.size() || !replaceText.size();

			if (disableReplaceButtons) {
				ImGui::BeginDisabled();
			}

			if (ImGui::Button(replaceButtonLabel.c_str(), ImVec2(button1Width, 0.0f))) {
				replace();
			}

			ImGui::SameLine();

			if (ImGui::Button(replaceAllButtonLabel.c_str(), ImVec2(button2Width, 0.0f))) {
				replaceAll();
			}

			if (disableReplaceButtons) {
				ImGui::EndDisabled();
			}
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();
		ImGui::SetCursorScreenPos(currentScreenPosition);
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

#if __APPLE__
		// Dear ImGui switches the Cmd(Super) and Ctrl keys on MacOS
		auto super = ImGui::IsKeyDown(ImGuiMod_Super);
		auto isCtrlShift = !ctrl && shift && !alt && super;
		auto isOptionalAltShift = !ctrl;
	#else
		auto isShiftAlt = !ctrl && shift && alt;
		auto isOptionalCtrlShift = !alt;
	#endif

		// cursor movements and selections
		if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_UpArrow)) { moveUp(1, shift); }
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_DownArrow)) { moveDown(1, shift); }

#if __APPLE__
		else if (isCtrlShift && ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) { shrinkSelectionsToCurlyBrackets(); }
		else if (isCtrlShift && ImGui::IsKeyPressed(ImGuiKey_RightArrow)) { growSelectionsToCurlyBrackets(); }
		else if (isOptionalAltShift && ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) { moveLeft(shift, alt); }
		else if (isOptionalAltShift && ImGui::IsKeyPressed(ImGuiKey_RightArrow)) { moveRight(shift, alt); }
#else
		else if (isShiftAlt && ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) { shrinkSelectionsToCurlyBrackets(); }
		else if (isShiftAlt && ImGui::IsKeyPressed(ImGuiKey_RightArrow)) { growSelectionsToCurlyBrackets(); }
		else if (isOptionalCtrlShift && ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) { moveLeft(shift, ctrl); }
		else if (isOptionalCtrlShift && ImGui::IsKeyPressed(ImGuiKey_RightArrow)) { moveRight(shift, ctrl); }
#endif

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
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_C)) { copy() ;}
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_Insert)) { copy(); }

		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_V)) { paste(); }
		else if (!readOnly && isShiftOnly && ImGui::IsKeyPressed(ImGuiKey_Insert)) { paste(); }
		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_Z)) { undo(); }
		else if (!readOnly && isShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_Z)) { redo(); }
		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_Y)) { redo(); }

		// remove text
		else if (!readOnly && isOptionalAlt && ImGui::IsKeyPressed(ImGuiKey_Delete)) { handleDelete(alt); }
		else if (!readOnly && isOptionalAlt && ImGui::IsKeyPressed(ImGuiKey_Backspace)) { handleBackspace(alt); }
		else if (!readOnly && isShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_K)) { removeSelectedLines(); }

		// text manipulation
		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_LeftBracket)) { deindentLines(); }
		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_RightBracket)) { indentLines(); }
		else if (!readOnly && isAltOnly && ImGui::IsKeyPressed(ImGuiKey_UpArrow)) { moveUpLines(); }
		else if (!readOnly && isAltOnly && ImGui::IsKeyPressed(ImGuiKey_DownArrow)) { moveDownLines(); }
		else if (!readOnly && language && isShortcut && ImGui::IsKeyPressed(ImGuiKey_Slash)) { toggleComments(); }

		// find/replace support
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_F)) { openFindReplace(); }
		else if (isShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_F)) { findAll(); }
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_G)) { findNext(); }

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
			for (int i = 0; i < io.InputQueueCharacters.size(); i++) {
				auto character = io.InputQueueCharacters[i];

				if (character == '\n' || character >= 32) {
					handleCharacter(character);
				}
			}

			io.InputQueueCharacters.resize(0);
		}
	}
}


//
//	TextEditor::handleMouseInteractions
//

void TextEditor::handleMouseInteractions() {
	// handle middle mouse button modes
	panning &= panMode && ImGui::IsMouseDown(ImGuiMouseButton_Middle);
	auto absoluteMousePos = ImGui::GetMousePos() - ImGui::GetWindowPos();

	if (panning && ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
		// handle middle mouse button panning
		auto windowSize = ImGui::GetWindowSize();
		auto mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
		float dragFactor = ImGui::GetIO().DeltaTime * 15.0f;
		ImVec2 autoPanMargin(glyphSize.x * 4.0f, glyphSize.y * 2.0f);

		if (absoluteMousePos.x < textOffset + autoPanMargin.x) {
			mouseDelta.x = (absoluteMousePos.x - (textOffset + autoPanMargin.x)) * dragFactor;

		} else if (absoluteMousePos.x > windowSize.x - verticalScrollBarSize - autoPanMargin.x) {
			mouseDelta.x = (absoluteMousePos.x - (windowSize.x - verticalScrollBarSize - autoPanMargin.x)) * dragFactor;
		}

		if (absoluteMousePos.y < autoPanMargin.y) {
			mouseDelta.y = (absoluteMousePos.y - autoPanMargin.y) * dragFactor;

		} else if (absoluteMousePos.y > windowSize.y - horizontalScrollBarSize - autoPanMargin.y) {
			mouseDelta.y = (absoluteMousePos.y - (windowSize.y - horizontalScrollBarSize - autoPanMargin.y)) * dragFactor;
		}

		ImGui::SetScrollX(ImGui::GetScrollX() - mouseDelta.x);
		ImGui::SetScrollY(ImGui::GetScrollY() - mouseDelta.y);
		ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);

	} else if (scrolling) {
		// handle middle mouse button scrolling
		float deadzone = glyphSize.x;
		auto offset = scrollStart - absoluteMousePos;
		offset.x = (offset.x < 0.0f) ? std::min(offset.x + deadzone, 0.0f) : std::max(offset.x - deadzone, 0.0f);
		offset.y = (offset.y < 0.0f) ? std::min(offset.y + deadzone, 0.0f) : std::max(offset.y - deadzone, 0.0f);

		float scrollFactor = ImGui::GetIO().DeltaTime * 5.0f;
		offset *= scrollFactor;

		ImGui::SetScrollX(ImGui::GetScrollX() - offset.x);
		ImGui::SetScrollY(ImGui::GetScrollY() - offset.y);

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) ||
			ImGui::IsMouseClicked(ImGuiMouseButton_Middle) ||
			ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {

			scrolling = false;
		}

	// ignore interactions when the editor is not hovered
	} else if (ImGui::IsWindowHovered()) {
		auto io = ImGui::GetIO();
		auto mousePos = ImGui::GetMousePos() - ImGui::GetCursorScreenPos();
		bool overLineNumbers = showLineNumbers && absoluteMousePos.x > lineNumberLeftOffset && absoluteMousePos.x < lineNumberRightOffset;
		bool overText = mousePos.x - ImGui::GetScrollX() > textOffset;

		auto mouseCoord = document.normalizeCoordinate(Coordinate(
			static_cast<int>(std::floor(mousePos.y / glyphSize.y)),
			static_cast<int>(std::round((mousePos.x - textOffset) / glyphSize.x))));

		auto mouseCoordAbs = document.normalizeCoordinate(Coordinate(
			static_cast<int>(std::floor(mousePos.y / glyphSize.y)),
			static_cast<int>(std::floor((mousePos.x - textOffset) / glyphSize.x))));

		// show text cursor if required
		if (ImGui::IsWindowFocused() && overText) {
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

			makeCursorVisible();

		} else if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {
			// start panning/scrolling mode on middle mouse click
			if (panMode) {
				panning = true;

			} else {
				scrolling = true;
				scrollStart = absoluteMousePos;
			}

		} else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
			// handle right clicks by setting up context menu (if required)
			if (overLineNumbers && lineNumberContextMenuCallback) {
				contextMenuLine = mouseCoordAbs.line;
				ImGui::OpenPopup("LineNumberContextMenu");

			} else if (overText && textContextMenuCallback) {
				contextMenuLine = mouseCoordAbs.line;
				contextMenuColumn = mouseCoordAbs.column;
				ImGui::OpenPopup("TextContextMenu");
			}

		} else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
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
				if (overText) {
					auto start = document.getStartOfLine(mouseCoord);
					auto end = document.getDown(start);
					cursors.updateCurrentCursor(start, end);
				}

			} else if (doubleClick) {
				// left mouse button double click
				if (overText) {
					auto codepoint = document.getCodePoint(mouseCoordAbs);
					bool handled = false;

					// select bracketed section (if required)
					if (CodePoint::isBracketOpener(codepoint)) {
						auto brackets = bracketeer.getEnclosingBrackets(document.getRight(mouseCoordAbs));

						if (brackets != bracketeer.end()) {
							if (ImGui::IsKeyDown(ImGuiMod_Shift)) {
								cursors.setCursor(brackets->start, document.getRight(brackets->end));

							} else {
								cursors.setCursor(document.getRight(brackets->start), brackets->end);
							}

							handled = true;
						}

					} else if (CodePoint::isBracketCloser(codepoint)) {
						auto brackets = bracketeer.getEnclosingBrackets(mouseCoordAbs);

						if (brackets != bracketeer.end()) {
							cursors.setCursor(brackets->start, document.getRight(brackets->end));
							handled = true;
						}
					}

					// select word if it wasn't a bracketed section
					if (!handled) {
						auto start = document.findWordStart(mouseCoordAbs);
						auto end = document.findWordEnd(mouseCoordAbs);
						cursors.updateCurrentCursor(start, end);
					}
				}

			} else if (click) {
				// left mouse button single click
				auto extendCursor = ImGui::IsKeyDown(ImGuiMod_Shift);

#if __APPLE__
				auto addCursor = ImGui::IsKeyDown(ImGuiMod_Alt);
#else
				auto addCursor = ImGui::IsKeyDown(ImGuiMod_Ctrl);
#endif

				if (overLineNumbers) {
					// handle line number clicks
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

					makeCursorVisible();

				} else if (overText) {
					// handle mouse clicks in text
					if (extendCursor) {
						cursors.updateCurrentCursor(mouseCoord);

					} else if (addCursor) {
						cursors.addCursor(mouseCoord);

					} else {
						cursors.setCursor(mouseCoord);
					}

					makeCursorVisible();
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
//	TextEditor::selectRegion
//

void TextEditor::selectRegion(int startLine, int startColumn, int endLine, int endColumn) {
	auto start = document.normalizeCoordinate(Coordinate(startLine, startColumn));
	auto end = document.normalizeCoordinate(Coordinate(endLine, endColumn));

	if (end < start) {
		std::swap(start, end);
	}

	cursors.setCursor(start, end);
}


//
//	TextEditor::selectToBrackets
//

void TextEditor::selectToBrackets(bool includeBrackets) {
	if (!showMatchingBrackets) {
		bracketeer.update(document);
	}

	for (auto& cursor : cursors) {
		auto bracket = bracketeer.getEnclosingBrackets(cursor.getSelectionStart());

		if (bracket != bracketeer.end()) {
			if (includeBrackets) {
				cursor.update(bracket->start, document.getRight(bracket->end));

			} else {
				cursor.update(document.getRight(bracket->start), bracket->end);
			}
		}
	}
}


//
//	TextEditor::growSelectionsToCurlyBrackets
//

void TextEditor::growSelectionsToCurlyBrackets() {
	if (!showMatchingBrackets) {
		bracketeer.update(document);
	}

	for (auto& cursor : cursors) {
		auto start = cursor.getSelectionStart();
		auto end = cursor.getSelectionEnd();
		auto startCodePoint = document.getCodePoint(document.getLeft(start));
		auto endCodePoint = document.getCodePoint(end);

		if (startCodePoint == CodePoint::openCurlyBracket && endCodePoint == CodePoint::closeCurlyBracket) {
			cursor.update(document.getLeft(start),document.getRight(end));

		} else {
			auto bracket = bracketeer.getEnclosingCurlyBrackets(start, end);

			if (bracket != bracketeer.end()) {
				cursor.update(document.getRight(bracket->start), bracket->end);
			}
		}
	}
}


//
//	TextEditor::shrinkSelectionsToCurlyBrackets
//

void TextEditor::shrinkSelectionsToCurlyBrackets() {
	if (!showMatchingBrackets) {
		bracketeer.update(document);
	}

	for (auto& cursor : cursors) {
		if (cursor.hasSelection()){
			auto start = cursor.getSelectionStart();
			auto end = cursor.getSelectionEnd();
			auto startCodePoint = document.getCodePoint(start);
			auto endCodePoint = document.getCodePoint(document.getLeft(end));

			if (startCodePoint == CodePoint::openCurlyBracket && endCodePoint == CodePoint::closeCurlyBracket) {
				cursor.update(document.getRight(start),document.getLeft(end));

			} else {
				auto bracket = bracketeer.getInnerCurlyBrackets(start, end);

				if (bracket != bracketeer.end()) {
					cursor.update(bracket->start, document.getRight(bracket->end));
				}
			}
		}
	}
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
		makeCursorVisible();
	}
}


//
//	TextEditor::redo
//

void TextEditor::redo() {
	if (transactions.canRedo()) {
		transactions.redo(document, cursors);
		makeCursorVisible();
	}
}


//
//	TextEditor::getCursor
//

void TextEditor::getCursor(int& line, int& column, size_t cursor) const {
	cursor = std::min(cursor, cursors.size() - 1);
	auto pos = cursors[cursor].getInteractiveEnd();
	line = pos.line;
	column = pos.column;
}


//
//	TextEditor::getCursor
//

void TextEditor::getCursor(int& startLine, int& startColumn, int& endLine, int& endColumn, size_t cursor) const {
	cursor = std::min(cursor, cursors.size() - 1);
	auto start = cursors[cursor].getSelectionStart();
	auto end = cursors[cursor].getSelectionEnd();
	startLine = start.line;
	startColumn = start.column;
	endLine = end.line;
	endColumn = end.column;
}


//
//	TextEditor::getCursorText
//

std::string TextEditor::getCursorText(size_t cursor) const {
	cursor = std::min(cursor, cursors.size() - 1);
	return document.getSectionText(cursors[cursor].getSelectionStart(), cursors[cursor].getSelectionEnd());
}


//
//	TextEditor::makeCursorVisible
//

void TextEditor::makeCursorVisible() {
	ensureCursorIsVisible = true;
	scrollToLineNumber = -1;
}


//
//	TextEditor::scrollToLine
//

void TextEditor::scrollToLine(int line, Scroll alignment) {
	ensureCursorIsVisible = false;
	scrollToLineNumber = line;
	scrollToAlignment = alignment;
}


//
//	TextEditor::selectFirstOccurrenceOf
//

void TextEditor::selectFirstOccurrenceOf(const std::string_view& text, bool caseSensitive, bool wholeWord) {
	Coordinate start, end;

	if (document.findText(Coordinate(0, 0), text, caseSensitive, wholeWord, start, end)) {
		cursors.setCursor(start, end);
		makeCursorVisible();

	} else {
		cursors.clearAdditional(true);
	}
}


//
//	TextEditor::selectNextOccurrenceOf
//

void TextEditor::selectNextOccurrenceOf(const std::string_view& text, bool caseSensitive, bool wholeWord) {
	Coordinate start, end;

	if (document.findText(cursors.getCurrent().getSelectionEnd(), text, caseSensitive, wholeWord, start, end)) {
		cursors.setCursor(start, end);
		makeCursorVisible();

	} else {
		cursors.clearAdditional(true);
	}
}


//
//	TextEditor::selectAllOccurrencesOf
//

void TextEditor::selectAllOccurrencesOf(const std::string_view& text, bool caseSensitive, bool wholeWord) {
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

		makeCursorVisible();

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

void TextEditor::replaceTextInCurrentCursor(const std::string_view& text) {
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

void TextEditor::replaceTextInAllCursors(const std::string_view& text) {
	auto transaction = startTransaction();
	insertTextIntoAllCursors(transaction, text);
	endTransaction(transaction);
}


//
//	TextEditor::openFindReplace
//

void TextEditor::openFindReplace() {
	findReplaceVisible = true;
	focusOnFind = true;
}


//
//	TextEditor::find
//

void TextEditor::find() {
	if (findText.size()) {
		selectNextOccurrenceOf(findText, caseSensitiveFind, wholeWordFind);
		focusOnEditor = true;
	}
}


//
//	TextEditor::findNext
//

void TextEditor::findNext() {
	if (findText.size()) {
		selectNextOccurrenceOf(findText, caseSensitiveFind, wholeWordFind);
		focusOnEditor = true;
	}
}


//
//	TextEditor::findAll
//

void TextEditor::findAll() {
	if (findText.size()) {
		selectAllOccurrencesOf(findText, caseSensitiveFind, wholeWordFind);
		focusOnEditor = true;
	}
}


//
//	TextEditor::replace
//

void TextEditor::replace() {
	if (findText.size()) {
		if (!cursors.anyHasSelection()) {
			selectNextOccurrenceOf(findText, caseSensitiveFind, wholeWordFind);
		}

		replaceTextInCurrentCursor(replaceText);
		selectNextOccurrenceOf(findText, caseSensitiveFind, wholeWordFind);
		focusOnEditor = true;
	}
}


//
//	TextEditor::replaceAll
//

void TextEditor::replaceAll() {
	if (findText.size()) {
		selectAllOccurrencesOf(findText, caseSensitiveFind, wholeWordFind);
		replaceTextInAllCursors(replaceText);
		focusOnEditor = true;
	}
}


//
//	TextEditor::addMarker
//

void TextEditor::addMarker(int line, ImU32 lineNumberColor, ImU32 textColor, const std::string_view& lineNumberTooltip, const std::string_view& textTooltip) {
	if (line >= 0 && line < document.lineCount()) {
		markers.emplace_back(lineNumberColor, textColor, lineNumberTooltip, textTooltip);
		document[line].marker = markers.size();
	}
}


//
//	TextEditor::clearMarkers
//

void TextEditor::clearMarkers() {
	for (auto& line : document) {
		line.marker = 0;
	}

	markers.clear();
}


//
//	TextEditor::moveUp
//

void TextEditor::moveUp(int lines, bool select) {
	for (auto& cursor : cursors) {
		cursor.update(document.getUp(cursor.getInteractiveEnd(), lines), select);
	}

	makeCursorVisible();
}


//
//	TextEditor::moveDown
//

void TextEditor::moveDown(int lines, bool select) {
	for (auto& cursor : cursors) {
		cursor.update(document.getDown(cursor.getInteractiveEnd(), lines), select);
	}

	makeCursorVisible();
}


//
//	TextEditor::moveLeft
//

void TextEditor::moveLeft(bool select, bool wordMode) {
	for (auto& cursor : cursors) {
		cursor.update(document.getLeft(cursor.getInteractiveEnd(), wordMode), select);
	}

	makeCursorVisible();
}


//
//	TextEditor::moveRight
//

void TextEditor::moveRight(bool select, bool wordMode) {
	for (auto& cursor : cursors) {
		cursor.update(document.getRight(cursor.getInteractiveEnd(), wordMode), select);
	}

	makeCursorVisible();
}


//
//	TextEditor::moveToTop
//

void TextEditor::moveToTop(bool select) {
	cursors.clearAdditional();
	cursors.updateCurrentCursor(document.getTop(), select);
	makeCursorVisible();
}


//
//	TextEditor::moveToBottom
//

void TextEditor::moveToBottom(bool select) {
	cursors.clearAdditional();
	cursors.updateCurrentCursor(document.getBottom(), select);
	makeCursorVisible();
}


//
//	TextEditor::moveToStartOfLine
//

void TextEditor::moveToStartOfLine(bool select) {
	cursors.clearAdditional();
	cursors.updateCurrentCursor(document.getStartOfLine(cursors.getCurrent().getInteractiveEnd()), select);
	makeCursorVisible();
}


//
//	TextEditor::moveToEndOfLine
//

void TextEditor::moveToEndOfLine(bool select) {
	cursors.clearAdditional();
	cursors.updateCurrentCursor(document.getEndOfLine(cursors.getCurrent().getInteractiveEnd()), select);
	makeCursorVisible();
}


//
//	TextEditor::moveTo
//

void TextEditor::moveTo(Coordinate coordinate, bool select) {
	cursors.clearAdditional();
	cursors.updateCurrentCursor(coordinate, select);
	makeCursorVisible();
}


//
//	TextEditor::handleCharacter
//

void TextEditor::handleCharacter(ImWchar character) {
	auto transaction = startTransaction();

	auto opener = character;
	auto isPaired = !overwrite && completePairedGlyphs && CodePoint::isPairOpener(opener);
	auto closer = CodePoint::toPairCloser(opener);

	// ignore input if it was the closing character for a pair that was automatically inserted
	if (completePairCloser) {
		if (completePairCloser == character && completePairLocation == cursors.getCurrent().getSelectionEnd()) {
			completePairCloser = 0;
			moveRight(false, false);
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

				// insert the closing glyph
				char utf8[4];
				auto end1 = insertText(transaction, end, std::string_view(utf8, CodePoint::write(utf8, closer)));
				cursors.adjustForInsert(cursor, start, end1);

				// insert the opening glyph
				auto end2 = insertText(transaction, start, std::string_view(utf8, CodePoint::write(utf8, opener)));
				cursors.adjustForInsert(cursor, start, end2);

				// update old selection
				cursor->update(Coordinate(start.line, start.column + 1), Coordinate(end.line, end.column + 1));
			}
		}

	} else if (isPaired) {
		// insert the requested pair
		char utf8[8];
		auto size = CodePoint::write(utf8, opener);
		size += CodePoint::write(utf8 + size, closer);
		std::string_view pair(utf8, size);

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
		char utf8[4];
		insertTextIntoAllCursors(transaction, std::string_view(utf8, CodePoint::write(utf8, character)));
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
			if (Coordinate(line, 0) != cursorEnd && document[line].size()) {
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
			// determine how many whitespaces are available at the start with a max of 4 columns
			int column = 0;
			size_t index = 0;

			while (column < 4 && index < document[line].size() && std::isblank(document[line][index].codepoint)) {
				column += document[line][index].codepoint == '\t' ? tabSize - (column % tabSize) : 1;
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
	auto comment = language->singleLineComment;

	// process all cursors
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto cursorStart = cursor->getSelectionStart();
		auto cursorEnd = cursor->getSelectionEnd();

		// process all lines in this cursor
		for (auto line = cursorStart.line; line <= cursorEnd.line; line++) {
			if (Coordinate(line, 0) != cursorEnd && document[line].size()) {
				// see if line starts with a comment (after possible leading whitespaces)
				size_t start = 0;
				size_t i = 0;

				while (start < document[line].size() && CodePoint::isWhiteSpace(document[line][start].codepoint)) {
					start++;
				}

				while (start + i < document[line].size() && i < comment.size() && document[line][start + i].codepoint == comment[i]) {
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

void TextEditor::filterSelections(std::function<std::string(std::string_view)> filter) {
	auto transaction = startTransaction();

	// process all cursors
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto start = cursor->getSelectionStart();
		auto end = cursor->getSelectionEnd();

		// process all lines in this cursor
		for (auto line = start.line; line <= end.line; line++) {
			if (Coordinate(line, 0) != end && document[line].size()) {
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
	FilterSelections([](const std::string_view& text) {
		std::string result;
		auto end = text.end();
		auto i = text.begin();
		char utf8[4];

		while (i < end) {
			ImWchar codepoint;
			i = CodePoint::read(i, end, &codepoint);
			result.append(utf8, CodePoint::write(utf8, CodePoint::toLower(codepoint)));
		}

		return result;
	});
}


//
//	TextEditor::selectionToUpperCase
//

void TextEditor::selectionToUpperCase() {
	FilterSelections([](const std::string_view& text) {
		std::string result;
		auto end = text.end();
		auto i = text.begin();
		char utf8[4];

		while (i < end) {
			ImWchar codepoint;
			i = CodePoint::read(i, end, &codepoint);
			result.append(utf8, CodePoint::write(utf8, CodePoint::toUpper(codepoint)));
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
	for (int i = 0; i < document.lineCount(); i++) {
		auto& line = document[i];
		size_t lineSize = line.size();
		size_t whitespace = std::numeric_limits<std::size_t>::max();
		bool done = false;

		// look for first non-whitespace glyph at the end of the line
		if (lineSize) {
			for (auto index = lineSize - 1; !done; index--) {
				if (CodePoint::isWhiteSpace(line[index].codepoint)) {
					whitespace = index;

					if (index == 0) {
						done = true;
					}

				} else {
					done = true;
				}
			}
		}

		// remove whitespaces (if required)
		if (whitespace != std::numeric_limits<std::size_t>::max()) {
			auto start = Coordinate(i, document.getColumn(line, whitespace));
			auto end = Coordinate(i, document.getColumn(line, lineSize));
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

void TextEditor::filterLines(std::function<std::string(std::string_view)> filter) {
	auto transaction = startTransaction();

	// process all the lines
	for (int i = 0; i < document.lineCount(); i++) {
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
	filterLines([this](const std::string_view& input) {
		auto tabSize = static_cast<size_t>(document.getTabSize());
		std::string output;
		auto end = input.end();
		auto i = input.begin();
		size_t pos = 0;

		while (i < end) {
			char utf8[4];
			ImWchar codepoint;
			i = CodePoint::read(i, end, &codepoint);

			if (codepoint == '\t') {
				auto spaces = tabSize - (pos % tabSize);
				output.append(spaces, ' ');
				pos += spaces;

			} else {
				output.append(utf8, CodePoint::write(utf8, codepoint));
				pos++;
			}
		}

		return output;
	});
}


//
//	TextEditor::spacesToTabs
//

void TextEditor::spacesToTabs() {
	FilterLines([this](const std::string_view& input) {
		auto tabSize = static_cast<size_t>(document.getTabSize());
		std::string output;
		auto end = input.end();
		auto i = input.begin();
		size_t pos = 0;
		size_t spaces = 0;

		while (i < end) {
			char utf8[4];
			ImWchar codepoint;
			i = CodePoint::read(i, end, &codepoint);

			if (codepoint == ' ') {
				spaces++;

			} else {
				while (spaces) {
					auto spacesUntilNextTab = tabSize - (pos % tabSize);

					if (spacesUntilNextTab == 1) {
						output += ' ';
						pos++;
						spaces--;

					} else if (spaces >= spacesUntilNextTab) {
						output += '\t';
						pos += spacesUntilNextTab;
						spaces -= spacesUntilNextTab;

					} else if (codepoint != '\t')
						while (spaces) {
							output += ' ';
							pos++;
							spaces--;
						}

					else {
						spaces = 0;
					}
				}

				if (codepoint == '\t') {
					output += '\t';
					pos += tabSize - (pos % tabSize);

				} else {
					output.append(utf8, CodePoint::write(utf8, codepoint));
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

void TextEditor::insertTextIntoAllCursors(std::shared_ptr<Transaction> transaction, const std::string_view& text) {
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
		ImWchar previousChar = index > 0 ? line[index - 1].codepoint : 0;
		ImWchar nextChar = index < line.size() ? line[index].codepoint : 0;

		// remove extra whitespaces if required
		if (CodePoint::isWhiteSpace(nextChar)) {
			while (index < line.size() && CodePoint::isWhiteSpace(line[index].codepoint)) {
				index++;
			}

			auto end = Coordinate(start.line, document.getColumn(start.line, index));
			deleteText(transaction, start, end);
			cursors.adjustForDelete(cursor, start, end);
		}

		// determine whitespace at start of current line
		std::string whitespace;

		for (size_t i = 0; i < line.size() && CodePoint::isWhiteSpace(line[i].codepoint); i++) {
			char utf8[4];
			whitespace.append(utf8, CodePoint::write(utf8, line[i].codepoint));
		}

		// determine text to insert
		std::string insert = "\n" + whitespace;
		auto newCursorIndex = static_cast<int>(whitespace.size());

		// handle special cases
		if (previousChar == CodePoint::openCurlyBracket || previousChar == CodePoint::openSquareBracket) {
			// add to an existing block
			insert += "\t";
			newCursorIndex++;

			if ((previousChar == CodePoint::openCurlyBracket && nextChar == CodePoint::closeCurlyBracket) ||
				(previousChar == CodePoint::openSquareBracket && nextChar == CodePoint::closeSquareBracket)) {

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

TextEditor::Coordinate TextEditor::insertText(std::shared_ptr<Transaction> transaction, Coordinate start, const std::string_view& text) {
	// update document, add transaction and return coordinate of end of insert
	// this function does not touch the cursors
	auto end = document.insertText(start, text);
	transaction->addInsert(start, end, text);
	makeCursorVisible();
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
	makeCursorVisible();
}


//
//	TextEditor::updatePalette
//

void TextEditor::updatePalette() {
	// Update palette with the current alpha from style
	paletteAlpha = ImGui::GetStyle().Alpha;

	for (size_t i = 0; i < static_cast<size_t>(Color::count); i++) {
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
		IM_COL32( 80,  80,  80, 255),	// whitespace
		IM_COL32( 70,  70,  70, 255),	// matchingBracketBackground
		IM_COL32(140, 140, 140, 255),	// matchingBracketActive
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
		IM_COL32( 170,  0, 220, 255),	// keyword
		IM_COL32( 65,   0, 255, 255),	// declaration
		IM_COL32( 40, 140,  90, 255),	// number
		IM_COL32(160,  32,  32, 255),	// string
		IM_COL32(  0,   0,   0, 255),	// punctuation
		IM_COL32( 96,  96,  64, 255),	// preprocessor
		IM_COL32( 64,  64,  64, 255),	// identifier
		IM_COL32( 16,  96,  96, 255),	// known identifier
		IM_COL32( 35, 135,   5, 255),	// comment
		IM_COL32(255, 255, 255, 255),	// background
		IM_COL32(  0,   0,   0, 255),	// cursor
		IM_COL32(  0,   0,  96,  64),	// selection
		IM_COL32(144, 144, 144, 144),	// whitespace
		IM_COL32(180, 180, 180, 144),	// matchingBracketBackground
		IM_COL32( 72,  72,  72, 255),	// matchingBracketActive
		IM_COL32( 70,   0, 250, 255),	// matchingBracketLevel1
		IM_COL32( 80, 160,  70, 255),	// matchingBracketLevel2
		IM_COL32(120,  60, 25, 255),	// matchingBracketLevel3
		IM_COL32(198,   8,  32, 255),	// matchingBracketError
		IM_COL32(  0,  80,  80, 255),	// line number
		IM_COL32(  0,   0,   0, 255),	// current line number
	}};

	return p;
}

TextEditor::Palette TextEditor::defaultPalette = TextEditor::GetDarkPalette();
