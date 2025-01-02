//	Color Text Editor
//	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "imgui.h"


//
//	OtTextWidget
//

class OtTextWidget {
public:
	// constructor
	OtTextWidget() { SetPalette(defaultPalette); }


	//
	// Below is the public API
	// Public member functions start with an uppercase character to be consistent with Dear ImGui
	//

	// access editor options
	inline void SetTabSize(int value) { tabSize = std::max(1, std::min(8, value)); }
	inline int GetTabSize() const { return tabSize; }
	inline void SetLineSpacing(float value) { lineSpacing = std::max(1.0f, std::min(2.0f, value)); }
	inline float GetLineSpacing() const { return lineSpacing; }
	inline void SetReadOnlyEnabled(bool value) { readOnly = value; }
	inline bool IsReadOnlyEnabled() const { return readOnly; }
	inline void SetAutoIndentEnabled(bool value) { autoIndent = value; }
	inline bool IsAutoIndentEnabled() const { return autoIndent; }
	inline void SetShowWhitespacesEnabled(bool value) { showWhitespaces = value; }
	inline bool IsShowWhitespacesEnabled() const { return showWhitespaces; }
	inline void SetShowLineNumbersEnabled(bool value) { showLineNumbers = value; }
	inline bool IsShowLineNumbersEnabled() const { return showLineNumbers; }
	inline void SetShowMatchingBrackets(bool value) { showMatchingBrackets = value; }
	inline bool IsShowingMatchingBrackets() const { return showMatchingBrackets; }
	inline void SetCompletePairedGlyphs(bool value) { completePairedGlyphs = value; }
	inline bool IsCompletingPairedGlyphs() const { return completePairedGlyphs; }

	// access text
	inline void SetText(const std::string& text) { setText(text); }
	inline std::string GetText() { return document.getText(); }
	inline bool IsEmpty() const { return document.size() == 1 && document[0].size() == 0; }
	inline int GetLineCount() const { return document.lines(); }

	// render the text editor in a Dear ImGui context
	inline void Render(const char* title, const ImVec2& size=ImVec2(), bool border=false) { render(title, size, border); }

	// manipulate selections/cursors
	inline void SetCursor(int line, int column) { moveTo(document.normalizeCoordinate(Coordinate(line, column), tabSize), false); }
	inline void SelectAll() { selectAll(); }
	inline void SelectLine(int line) { if (line >= 0 && line < document.lines()) selectLine(line); }
	inline void SelectLines(int start, int end) { if (start >= 0 && end < document.lines() && start <= end) selectLines(start, end); }
	inline bool AnyCursorHasSelection() const { return cursors.anyHasSelection(); }
	inline bool AllCursorsHaveSelection() const { return cursors.allHaveSelection(); }
	inline void ClearCursors() { cursors.clearAll(); }

	// clipboard actions
	inline void Cut() { if (!readOnly) cut(); }
	inline void Copy() const { copy(); }
	inline void Paste() { if (!readOnly) paste(); }
	inline void Undo() { if (!readOnly) undo(); }
	inline void Redo() { if (!readOnly) redo(); }
	inline bool CanUndo() { return !readOnly && transactions.canUndo(); };
	inline bool CanRedo() { return !readOnly && transactions.canRedo(); };
	inline size_t GetUndoIndex() { return transactions.getUndoIndex(); };

	// find/replace support
	inline void SelectFirstOccurrenceOf(const std::string& text, bool caseSensitive=true, bool wholeWord=false) { selectFirstOccurrenceOf(text, caseSensitive, wholeWord); }
	inline void SelectNextOccurrenceOf(const std::string& text, bool caseSensitive=true, bool wholeWord=false) { selectNextOccurrenceOf(text, caseSensitive, wholeWord); }
	inline void SelectAllOccurrencesOf(const std::string& text, bool caseSensitive=true, bool wholeWord=false) { selectAllOccurrencesOf(text, caseSensitive, wholeWord); }
	inline void ReplaceTextInCurrentCursor(const std::string& text) { if (!readOnly) replaceTextInCurrentCursor(text); }
	inline void ReplaceTextInAllCursors(const std::string& text) { if (!readOnly) replaceTextInAllCursors(text); }

	// access error markers
	inline void SetErrorMarkers(const std::map<int, std::string>& markers) { errorMarkers = markers; }
	inline void ClearErrorMarkers() { errorMarkers.clear(); }
	inline bool HasErrorMarkers() const { return errorMarkers.size() != 0; }

	// useful editor functions
	inline void StripTrailingWhitespaces() { if (!readOnly) stripTrailingWhitespaces(); }
	inline void FilterLines(std::function<std::string(std::string)> filter) { if (!readOnly) filterLines(filter); }
	inline void TabsToSpaces() { if (!readOnly) tabsToSpaces(); }
	inline void SpacesToTabs() { if (!readOnly) spacesToTabs(); }

	// color palete support
	enum class Color {
		standard,
		keyword,
		number,
		string,
		character,
		punctuation,
		preprocessor,
		identifier,
		knownIdentifier,
		comment,
		background,
		cursor,
		selection,
		errorMarker,
		whitespace,
		lineNumber,
		currentLineNumber,
		count
	};

	typedef std::array<ImU32, static_cast<size_t>(Color::count)> Palette;

	inline void SetPalette(const Palette& palette) { paletteBase = palette; paletteAlpha = -1.0f; }
	inline const Palette& GetPalette() const { return paletteBase; }
	inline static void SetDefaultPalette(const Palette& aValue) { defaultPalette = aValue; }
	inline static Palette& GetDefaultPalette() { return defaultPalette; }

	static const Palette& GetDarkPalette();
	static const Palette& GetLightPalette();
	static const Palette& GetRetroBluePalette();
	static const Palette& GetMarianaPalette();

private:
	//
	// Below is the private API
	// Private members (function and variables) start with an lowercase character
	//

	// editor options
	int tabSize = 4;
	float lineSpacing = 1.0f;
	bool readOnly = false;
	bool autoIndent = true;
	bool showWhitespaces = true;
	bool showLineNumbers = true;
	bool showMatchingBrackets = true;
	bool completePairedGlyphs = false;

	// Represents a character coordinate from the user's point of view, i. e. consider an uniform grid (assuming fixed-width font)
	// on the screen as it is rendered, and each cell has its own coordinate, starting from 0.
	//
	// Tabs are counted as [1..mTabSize] count empty spaces, depending on how many space is necessary to reach the next tab stop.
	// For example, coordinate (1, 5) represents the character 'B' in a line "\tABC", when mTabSize = 4,
	// because it is rendered as "    ABC" on the screen.
	struct Coordinate
	{
		Coordinate() = default;
		Coordinate(int l, int c) : line(l), column(c) {}

		inline bool operator ==(const Coordinate& o) const { return line == o.line && column == o.column; }
		inline bool operator !=(const Coordinate& o) const { return line != o.line || column != o.column; }
		inline bool operator <(const Coordinate& o) const { return line != o.line ? line < o.line : column < o.column; }
		inline bool operator >(const Coordinate& o) const { return line != o.line ? line > o.line : column > o.column; }
		inline bool operator <=(const Coordinate& o) const { return line != o.line ? line < o.line : column <= o.column; }
		inline bool operator >=(const Coordinate& o) const { return line != o.line ? line > o.line : column >= o.column; }

		inline Coordinate operator -(const Coordinate& o) const { return Coordinate(line - o.line, column - o.column); }
		inline Coordinate operator +(const Coordinate& o) const { return Coordinate(line + o.line, column + o.column); }

		int line = 0;
		int column = 0;
	};

	// a single cursor
	class Cursor {
	public:
		// constructors
		Cursor() = default;
		Cursor(Coordinate coordinate) : start(coordinate), end(coordinate) {}
		Cursor(Coordinate s, Coordinate e) : start(s), end(e) {}

		// update the cursor
		inline void update(Coordinate coordinate) { end = coordinate; updated = true; }
		inline void update(Coordinate s, Coordinate e) { start = s; end = e; updated = true; }
		inline void update(Coordinate coordinate, bool keep) { if (keep) update(coordinate); else update(coordinate, coordinate); updated = true; }

		// adjust cursor for insert/delete operations
		// (these functions assume that insert or delete happened before the cursor)
		void adjustForInsert(Coordinate deleteStart, Coordinate deleteEnd);
		void adjustForDelete(Coordinate insertStart, Coordinate insertEnd);

		// access cursor properties
		inline Coordinate getInteractiveStart() const { return start; }
		inline Coordinate getInteractiveEnd() const { return end; }
		inline Coordinate getSelectionStart() const { return start < end ? start : end; }
		inline Coordinate getSelectionEnd() const { return start > end ? start : end; }
		inline bool hasSelection() const { return start != end; }

		inline void setCurrent(bool value) { current = value; }
		inline bool isCurrent() const { return current; }

		inline void setUpdated(bool value) { updated = value; }
		inline bool isUpdated() const { return updated; }

	private:
		// helper functions
		Coordinate adjustCoordinateForInsert(Coordinate coordinate, Coordinate start, Coordinate end);
		Coordinate adjustCoordinateForDelete(Coordinate coordinate, Coordinate start, Coordinate end);

		// properties
		Coordinate start = {0, 0};
		Coordinate end = {0, 0};
		bool current = true;
		bool updated = true;
	};

	// the current list of cursors
	class Cursors : public std::vector<Cursor> {
	public:
		// constructor
		Cursors() { clearAll(); }

		// erase all cursors and specify a new one
		inline void setCursor(Coordinate coordinate) { setCursor(coordinate, coordinate); }
		void setCursor(Coordinate start, Coordinate end);

		// add a cursor to the list
		inline void addCursor(Coordinate c) { addCursor(c, c); }
		void addCursor(Coordinate start, Coordinate end);

		// update the cursor last added
		inline void updateCursor(Coordinate coordinate) { at(current).update(coordinate); }
		inline void updateCursor(Coordinate start, Coordinate end) { at(current).update(start, end); }
		inline void updateCursor(Coordinate coordinate, bool keep) { at(current).update(coordinate, keep); }

		// do we have multiple cursors in this collection
		inline bool hasMultiple() const { return size() > 1; }

		// do any cursors have a selection
		bool anyHasSelection() const;

		// do all cursors have a selection
		bool allHaveSelection() const;

		// do any cursors have an update
		bool anyHasUpdate() const;

		// clear the selection and create the default cursor
		void clearAll();

		// clear all additional cursors
		void clearAdditional();

		// clear all updated flags
		void clearUpdated();

		// get current cursor
		inline Cursor& getCurrent() { return at(current); }

		// update cursors
		void update();

	private:
		size_t current = 0;
	} cursors;

	// a single character
	struct Glyph {
		// constructors
		Glyph() = default;
		Glyph(char32_t cp, Color col) : codepoint(cp), color(col) {}

		// properties
		char32_t codepoint = 0;
		Color color = Color::standard;
	};

	// a single line in the editor
	class Line : public std::vector<Glyph> {
	public:
		friend class Document;
		// get number of glyphs
		inline int glyphs() const { return static_cast<int>(size()); }

		// determine the maximum column number for this line
		int getMaxColumn(int tabSize) const;

		// translate visible column to line index (and visa versa)
		int columnToIndex(int column, int tabSize) const;
		int indexToColumn(int index, int tabSize) const;
	};

	// the document being edited (Lines of Glyphs)
	class Document : public std::vector<Line> {
	public:
		// constructor
		Document() {
			// create empty line
			emplace_back();
		}

		// access/manipulate document text
		void setText(const std::string& text);
		std::string getText() const;
		std::string getSectionText(Coordinate start, Coordinate end, int tabSize) const;
		std::string getLineText(int line, int tabSize) const;
		Coordinate insertText(Coordinate from, const std::string& text, int tabSize);
		void deleteText(Coordinate start, Coordinate end, int tabSize);

		// get number of lines
		inline int lines() const { return static_cast<int>(size()); }

		// determine the maximum column number for this document
		int getMaxColumn(int tabSize) const;

		// coordinate operations in context of document
		Coordinate getUp(Coordinate from, int lines, int tabSize) const;
		Coordinate getDown(Coordinate from, int lines, int tabSize) const;
		Coordinate getLeft(Coordinate from, bool wordMode, int tabSize) const;
		Coordinate getRight(Coordinate from, bool wordMode, int tabSize) const;
		Coordinate getTop() const;
		Coordinate getBottom(int tabSize) const;
		Coordinate getHome(Coordinate from) const;
		Coordinate getEnd(Coordinate from, int tabSize) const;
		Coordinate findWordStart(Coordinate from, int tabSize) const;
		Coordinate findWordEnd(Coordinate from, int tabSize) const;
		Coordinate normalizeCoordinate(Coordinate coordinate, int tabSize) const;
		bool isEndOfLine(Coordinate from, int tabSize) const;
		inline bool isLastLine(int line) const { return line == lines() - 1; }

	} document;

	// single action to be performed on text as part of a larger transaction
	struct Action {
		// action types
		enum class Type {
			insertText,
			deleteText
		};

		// constructors
		Action() = default;
		Action(Type t, Coordinate s, Coordinate e, const std::string& txt) : type(t), start(s), end(e), text(txt) {}

		// properties
		Type type;
		Coordinate start;
		Coordinate end;
		std::string text;
	};

	// a collection of actions that for a complete transaction
 	class Transaction : public std::vector<Action> {
	public:
		// access state before/after transactions
		inline void setBeforeState(const Cursors& cursors) { before = cursors; }
		inline const Cursors& getBeforeState() const { return before; }
		inline void setAfterState(const Cursors& cursors) { after = cursors; }
		inline const Cursors& getAfterState() const { return after; }

		// add actions by type
		void addInsert(Coordinate start, Coordinate end, std::string text) { emplace_back(Action::Type::insertText, start, end, text); };
		void addDelete(Coordinate start, Coordinate end, std::string text) { emplace_back(Action::Type::deleteText, start, end, text); };

		// get number of actions
		inline int actions() const { return static_cast<int>(size()); }

	private:
		// properties
		Cursors before;
		Cursors after;
	};

	// transaction list to support do/undo/redo
 	class Transactions : public std::vector<std::shared_ptr<Transaction>> {
	public:
		// create a new transaction
		static inline std::shared_ptr<Transaction> create() { return std::make_shared<Transaction>(); }

		// add a transaction to the list, execute it and make it undoable
		void add(std::shared_ptr<Transaction> transaction);

		// undo the last transction
		void undo(Document& document, Cursors& cursors, int tabSize);

		// redo the last undone transction;
		void redo(Document& document, Cursors& cursors, int tabSize);

		// get status information
		inline size_t getUndoIndex() const { return undoIndex; }
		inline bool canUndo() const { return undoIndex > 0; }
		inline bool canRedo() const { return undoIndex < size(); }

	private:
		size_t undoIndex = 0;
	} transactions;

	// set the editor's text
	void setText(const std::string& text);

	// render the text editor in a Dear ImGui context
	void render(const char* title, const ImVec2& size, bool border);

	// keyboard and mouse interactions
	void handleKeyboardInputs();
	void handleMouseInteractions();

	// manipulate selections/cursors
	void selectAll();
	void selectLine(int line);
	void selectLines(int startLine, int endLine);

	// clipboard actions
	void cut();
	void copy() const;
	void paste();
	void undo();
	void redo();

	// find/replace support
	void selectFirstOccurrenceOf(const std::string& text, bool caseSensitive, bool wholeWord);
	void selectNextOccurrenceOf(const std::string& text, bool caseSensitive, bool wholeWord);
	void selectAllOccurrencesOf(const std::string& text, bool caseSensitive, bool wholeWord);
	void replaceTextInCurrentCursor(const std::string& text);
	void replaceTextInAllCursors(const std::string& text);

	// cursor/selection functions
	void moveUp(int lines, bool select);
	void moveDown(int lines, bool select);
	void moveLeft(bool select, bool wordMode);
	void moveRight(bool select, bool wordMode);
	void moveTop(bool select);
	void moveBottom(bool select);
	void moveHome(bool select);
	void moveEnd(bool select);
	void moveTo(Coordinate coordinate, bool select);

	// set line visibility
	void makeCursorVisible();

	// add/delete characters
	void handleCharacter(char32_t codepoint);
	void handleBackspace(bool wordMode);
	void handleDelete(bool wordMode);

	// add/delete lines
	void removeSelectedLines();
	void insertLineAbove();
	void insertLineBelow();

	// add/delete text (affecting all cursors)
	void insertTextAtCursors(std::shared_ptr<Transaction> transaction, const std::string& text);
	void deleteTextAtCursors(std::shared_ptr<Transaction> transaction);

	// transaction functions
	std::shared_ptr<Transaction> startTransaction();
	bool endTransaction(std::shared_ptr<Transaction> transaction);
	Coordinate insertText(std::shared_ptr<Transaction> transaction, Coordinate from, const std::string& text);
	void deleteText(std::shared_ptr<Transaction> transaction, Coordinate start, Coordinate end);

	// useful editor functions
	void stripTrailingWhitespaces();
	void filterLines(std::function<std::string(std::string)> filter);
	void tabsToSpaces();
	void spacesToTabs();

	// rendering context
	ImFont* font;
	float fontSize;
	ImVec2 glyphSize;
	float textStart;
	int longestLine;
	float visibleHeight;
	int visibleLines;
	int firstVisibleLine;
	int lastVisibleLine;
	float visibleWidth;
	int visibleColumns;
	int firstVisibleColumn;
	int lastVisibleColumn;
	bool ensureCursorIsVisible = false;
	float cursorAnimationTimer = 0.0f;
	std::map<int, std::string> errorMarkers;

	static constexpr int leftMargin = 2;
	static constexpr int lineNumberMargin = 2;
	static constexpr int cursorWidth = 1;

	// interaction context
	float lastClickTime = -1.0f;

	// color palette support
	inline ImU32 getColor(Color color) { return palette[static_cast<size_t>(color)]; }
	void updatePalette();

	static Palette defaultPalette;
	Palette paletteBase;
	Palette palette;
	float paletteAlpha;
};
