//	TextDiff - A syntax highlighting text diff widget for Dear ImGui.
//	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "TextEditor.h"


//
//	TextDiff
//

class TextDiff : public TextEditor {
public:
	// constructor
	TextDiff();

	// specify the text to be compared (using UTF-8 encoded strings)
	void SetText(const std::string_view& left, const std::string_view& right);

	// specify a new language
	void SetLanguage(const Language* l);

	// specify the background color for added/deleted lines
	inline void SetColors(ImU32 ac, ImU32 dc) { addedColor = ac; deletedColor = dc; }

	// block/hide certain API calls
	inline void SetReadOnlyEnabled(bool /* value */) {}
	inline void SetShowLineNumbersEnabled(bool /* value */) {}
	inline void SetShowMatchingBrackets(bool /* value */) {}

	inline void AddMarker(int /* line */, ImU32 /* lineNumberColor */, ImU32 /* textColor */, const std::string_view& /* lineNumberTooltip */, const std::string_view& /* textTooltip */) {}
	inline void ClearMarkers() {}

	inline void SetLineDecorator(float /* width */, std::function<void(Decorator& decorator)> /* callback */) {}
	inline void ClearLineDecorator() {}

	inline void SetLineNumberContextMenuCallback(std::function<void(int line)> /* callback */) {}
	inline void ClearLineNumberContextMenuCallback() {}

	inline void SetTextContextMenuCallback(std::function<void(int line, int column)> /* callback */) {}
	inline void ClearTextContextMenuCallback() {}

private:
	// properties
	TextEditor::Document leftDocument;
	TextEditor::Document rightDocument;

	int leftLineNumberDigits;
	int rightLineNumberDigits;

	enum class Status {
		common,
		added,
		deleted
	};

	class LineInfo {
	public:
		LineInfo(size_t l, size_t r, Status s) : leftLine(l), rightLine(r), status(s) {}

		size_t leftLine;
		size_t rightLine;
		Status status;
	};

	std::vector<LineInfo> lineInfo;

	// colors
	ImU32 addedColor = IM_COL32(0, 128, 32, 100);
	ImU32 deletedColor = IM_COL32(128, 0, 32, 100);

	// split string into lines
	void splitLines(std::vector<std::string_view>& result, const std::string_view& text);

	// create a combined view of the two documents and their differences
	void createCombinedView();

	// line decorator
	void decorateLine(TextEditor::Decorator& decorator);
};
