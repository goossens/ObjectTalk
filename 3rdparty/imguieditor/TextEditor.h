//	TextEditor - A syntax highlighting text editor for ImGui
//	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <array>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "imgui.h"


//
//	TextEditor
//

class TextEditor {
public:
	// constructor
	TextEditor() { SetPalette(defaultPalette); }


	//
	// Below is the public API
	// Public member functions start with an uppercase character to be consistent with Dear ImGui
	//

	// access editor options
	inline void SetTabSize(int value) { document.setTabSize(std::max(1, std::min(8, value))); }
	inline int GetTabSize() const { return document.getTabSize(); }
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

	// access text (using UTF-8 encoded strings)
	inline void SetText(const std::string& text) { setText(text); }
	inline std::string GetText() { return document.getText(); }
	inline bool IsEmpty() const { return document.size() == 1 && document[0].size() == 0; }
	inline int GetLineCount() const { return document.lines(); }

	// render the text editor in a Dear ImGui context
	inline void Render(const char* title, const ImVec2& size=ImVec2(), bool border=false) { render(title, size, border); }

	// manipulate cursors and selections
	inline void SetCursor(int line, int column) { moveTo(document.normalizeCoordinate(Coordinate(line, column)), false); }
	inline void SelectAll() { selectAll(); }
	inline void SelectLine(int line) { if (line >= 0 && line < document.lines()) selectLine(line); }
	inline void SelectLines(int start, int end) { if (start >= 0 && end < document.lines() && start <= end) selectLines(start, end); }
	inline void AddNextOccurrence() { addNextOccurrence(); }
	inline void SelectAllOccurrences() { selectAllOccurrences(); }
	inline bool AnyCursorHasSelection() const { return cursors.anyHasSelection(); }
	inline bool AllCursorsHaveSelection() const { return cursors.allHaveSelection(); }
	inline bool CurrentCursorHasSelection() const { return cursors.currentCursorHasSelection(); }
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
	inline void IndentLines() { if (!readOnly) indentLines(); }
	inline void DeindentLines() { if (!readOnly) deindentLines(); }
	inline void MoveUpLines() { if (!readOnly) moveUpLines(); }
	inline void MoveDownLines() { if (!readOnly) moveDownLines(); }
	inline void ToggleComments() { if (!readOnly && document.hasLanguage()) toggleComments(); }

	inline void StripTrailingWhitespaces() { if (!readOnly) stripTrailingWhitespaces(); }
	inline void FilterLines(std::function<std::string(std::string)> filter) { if (!readOnly) filterLines(filter); }
	inline void TabsToSpaces() { if (!readOnly) tabsToSpaces(); }
	inline void SpacesToTabs() { if (!readOnly) spacesToTabs(); }

	// color palete support
	enum class Color : char {
		standard,
		keyword,
		declaration,
		number,
		string,
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
		matchingBracket1,
		matchingBracket2,
		matchingBracket3,
		matchingBracketError,
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

	// iterator used in language specific tokonizer
	class Iterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = ImWchar;
		using pointer = ImWchar*;
		using reference = ImWchar&;

		reference operator*() const;
		pointer operator->() const;
		inline Iterator& operator++() { index++; return *this; }
		inline Iterator operator++(int) { Iterator tmp = *this; index++; return tmp; }
		inline int operator-(const Iterator& a) { return index - a.index; }
		inline friend bool operator== (const Iterator& a, const Iterator& b) { return a.index == b.index; };
		inline friend bool operator!= (const Iterator& a, const Iterator& b) { return !(a == b); };
		inline friend bool operator< (const Iterator& a, const Iterator& b) { return a.index < b.index; };

	private:
		// private constructors
		friend class TextEditor;
		Iterator() = default;
		Iterator(void* l, int i) : line(l), index(i) {}

		// properties
		void* line;
		int index;
	};

	// language support
	struct Language {
		std::string name;
		std::string commentStart;
		std::string commentEnd;
		std::string singleLineComment;
		std::string singleLineCommentAlt;
		ImWchar preprocess = 0;
		bool hasSingleQuoteStrings = false;
		bool hasDoubleQuoteStrings = false;
		ImWchar stringEscape = 0;
		std::unordered_set<std::string> keywords;
		std::unordered_set<std::string> declarations;
		std::unordered_set<std::string> identifiers;
		std::function<bool(ImWchar)> isPunctuation;
		std::function<bool(ImWchar)> isWord;
		std::function<Iterator(Iterator start, Iterator end)> getIdentifier;
		std::function<Iterator(Iterator start, Iterator end)> getNumber;

		static const Language& C();
		static const Language& Cpp();
		static const Language& Cs();
		static const Language& Hlsl();
		static const Language& Glsl();
		static const Language& Python();
		static const Language& AngelScript();
		static const Language& Lua();
		static const Language& Sql();
		static const Language& Json();
		static const Language& Markdown();

		static bool isCStylePunctuation(ImWchar character);
		static bool isCStyleWordCharacter(ImWchar character);
		static Iterator getCStyleIdentifier(Iterator start, Iterator end);
		static Iterator getCStyleNumber(Iterator start, Iterator end);

	};

	inline void SetLanguage(const Language& language) { document.setLanguage(language); }
	inline const Language& GetLanguage() const { return document.getLanguage(); };
	inline bool HasLanguage() const { return document.hasLanguage(); }
	inline std::string GetLanguageName() const { return document.getLanguageName(); }

private:
	//
	// below is the private API
	// private members (function and variables) start with an lowercase character
	//

	// represent a character coordinate from the user's point of view, i. e. consider an uniform grid
	// on the screen as it is rendered, and each cell has its own coordinate, starting from 0
	//
	// tabs are counted as [1..tabsize] count spaces, depending on how many spaces are necessary to
	// reach the next tab stop
	//
	// for example, coordinate (1, 5) represents the character 'B' in a line "\tABC", when tabsize = 4,
	// because it is rendered as "    ABC" on the screen

	struct Coordinate {
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

		static Coordinate invalid() { static Coordinate invalid(-1, -1); return invalid; }
		inline bool isValid() const { return line >= 0 and column >=0; }

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
		// (these functions assume that insert or delete points are before the cursor)
		void adjustForInsert(Coordinate insertStart, Coordinate insertEnd);
		void adjustForDelete(Coordinate deleteStart, Coordinate deleteEnd);

		// access cursor properties
		inline Coordinate getInteractiveStart() const { return start; }
		inline Coordinate getInteractiveEnd() const { return end; }
		inline Coordinate getSelectionStart() const { return start < end ? start : end; }
		inline Coordinate getSelectionEnd() const { return start > end ? start : end; }
		inline bool hasSelection() const { return start != end; }

		inline void resetToStart() { update(getSelectionStart(), getSelectionStart()); }
		inline void resetToEnd() { update(getSelectionEnd(), getSelectionEnd()); }

		inline void setMain(bool value) { main = value; }
		inline bool isMain() const { return main; }

		inline void setCurrent(bool value) { current = value; }
		inline bool isCurrent() const { return current; }

		inline void setUpdated(bool value) { updated = value; }
		inline bool isUpdated() const { return updated; }

	private:
		// helper functions
		Coordinate adjustCoordinateForInsert(Coordinate coordinate, Coordinate start, Coordinate end);
		Coordinate adjustCoordinateForDelete(Coordinate coordinate, Coordinate start, Coordinate end);

		// properties
		Coordinate start{0, 0};
		Coordinate end{0, 0};
		bool main = false;
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

		// update the current cursor (the one last added)
		inline void updateCurrentCursor(Coordinate coordinate) { at(current).update(coordinate); }
		inline void updateCurrentCursor(Coordinate start, Coordinate end) { at(current).update(start, end); }
		inline void updateCurrentCursor(Coordinate coordinate, bool keep) { at(current).update(coordinate, keep); }

		// check cursor status
		inline bool hasMultiple() const { return size() > 1; }
		bool anyHasSelection() const;
		bool allHaveSelection() const;
		inline bool mainCursorHasSelection() const { return at(main).hasSelection(); }
		inline bool currentCursorHasSelection() const { return at(current).hasSelection(); }
		inline bool mainHasUpdate() const  { return at(main).isUpdated(); }
		bool anyHasUpdate() const;

		// clear the selections and create the default cursor
		void clearAll();

		// clear all additional cursors
		void clearAdditional(bool reset=false);

		// clear all updated flags
		void clearUpdated();

		// get main/current cursor
		inline Cursor& getMain() { return at(main); }
		inline Cursor& getCurrent() { return at(current); }
		inline iterator getCurrentAsIterator() { return begin() + current; }

		// update cursors
		void update();

		// adjust cursors for insert/delete operations
		// (these functions assume that insert or delete points are before the cursor)
		void adjustForInsert(iterator start, Coordinate insertStart, Coordinate insertEnd);
		void adjustForDelete(iterator start, Coordinate deleteStart, Coordinate deleteEnd);

	private:
		size_t main = 0;
		size_t current = 0;
	} cursors;

	// a single character
	struct Glyph {
		// constructors
		Glyph() = default;
		Glyph(ImWchar ch) : character(ch) {}
		Glyph(ImWchar ch, Color col) : character(ch), color(col) {}

		// properties
		ImWchar character = 0;
		Color color = Color::standard;
	};

	// tokenizer state
	enum class State {
		inText,
		inComment,
		inSingleQuoteString,
		inDoubleQuoteString
	};

	// a single line in the editor
	class Line : public std::vector<Glyph> {
	public:
		friend class Document;
		// get number of glyphs
		inline int glyphs() const { return static_cast<int>(size()); }

		// determine the maximum column number for this line
		int maxColumn(int tabSize) const;

		// translate visible column to line index (and visa versa)
		int columnToIndex(int column, int tabSize) const;
		int indexToColumn(int index, int tabSize) const;

		// see if text at iterator matches (this only works for latin-1 codepoints at the moment)
		inline bool matches(iterator start, const std::string& text) {
			for (auto character : text) {
				if ((start++)->character != character) {
					return false;
				}
			}

			return true;
		}

		// set glyph colors
		inline void setColor(iterator start, iterator end, Color color) {
			while (start < end) {
				(start++)->color = color;
			}
		}

		// state at start of line
		State state = State::inText;
	};

	// the document being edited (Lines of Glyphs)
	class Document : public std::vector<Line> {
	public:
		// constructor
		Document() {
			// create empty line
			emplace_back();
		}

		// access document's tab size
		inline void setTabSize(int ts) { tabSize = ts; }
		inline int getTabSize() const { return tabSize; }

		// manipulate document text (strings should be UTF-8 encoded)
		void setText(const std::string& text);
		Coordinate insertText(Coordinate start, const std::string& text);
		void deleteText(Coordinate start, Coordinate end);

		// access document text (strings are UTF-8 encoded)
		std::string getText() const;
		std::string getSectionText(Coordinate start, Coordinate end) const;
		std::string getLineText(int line) const;

		// get number of lines
		inline int lines() const { return static_cast<int>(size()); }

		// determine maximum column for this document or just a line
		int maxColumn() const;
		inline int maxColumn(int line) const { return at(line).maxColumn(tabSize); }
		inline int maxColumn(const Line& line) const  { return line.maxColumn(tabSize); }

		// translate visible column to line index (and visa versa)
		inline int getIndex(Coordinate coordinate) const { return at(coordinate.line).columnToIndex(coordinate.column, tabSize); }
		inline int getIndex(const Line& line, int column) const { return line.columnToIndex(column, tabSize); }
		inline int getIndex(const iterator line, int column) const { return line->columnToIndex(column, tabSize); }
		inline int getColumn(int line, int index) const { return at(line).indexToColumn(index, tabSize); }
		inline int getColumn(const Line& line, int index) const { return line.indexToColumn(index, tabSize); }
		inline int getColumn(const iterator line, int index) const { return line->indexToColumn(index, tabSize); }

		// coordinate operations in context of document
		Coordinate getUp(Coordinate from, int lines=1) const;
		Coordinate getDown(Coordinate from, int lines=1) const;
		Coordinate getLeft(Coordinate from, bool wordMode=false) const;
		Coordinate getRight(Coordinate from, bool wordMode=false) const;
		Coordinate getTop() const;
		Coordinate getBottom() const;
		Coordinate getStartOfLine(Coordinate from) const;
		Coordinate getEndOfLine(Coordinate from) const;
		inline Coordinate getNextLine(Coordinate from) const { return getRight(getEndOfLine(from)); }

		// search in document
		Coordinate findWordStart(Coordinate from) const;
		Coordinate findWordEnd(Coordinate from) const;
		bool findText(Coordinate from, const std::string& text, bool caseSensitive, bool wholeWord, Coordinate& start, Coordinate& end) const;

		// see if document was updated this frame
		inline bool isUpdated() const { return updated; }
		inline void resetUpdated() { updated = false; }

		// utility functions
		bool isWholeWord(Coordinate start, Coordinate end) const;
		inline bool isEndOfLine(Coordinate from) const { return getIndex(from) == at(from.line).glyphs(); }
		inline bool isLastLine(int line) const { return line == lines() - 1; }
		Coordinate normalizeCoordinate(Coordinate coordinate) const;

		// language support
		void setLanguage(const Language& language);
		inline const Language& getLanguage() const { return *language; }
		inline bool hasLanguage() const { return language != nullptr; }
		inline std::string getLanguageName() const {  return language == nullptr ? "None" : language->name; }

	private:
		int tabSize = 4;
		const Language* language = nullptr;
		bool updated = false;

		// colorizer
		State colorize(Line& line);
		void colorize(int start, int end);
		void colorize();

	} document;

	// single action to be performed on text as part of a larger transaction
	struct Action {
		// action types
		enum class Type : char {
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
		void undo(Document& document, Cursors& cursors);

		// redo the last undone transction;
		void redo(Document& document, Cursors& cursors);

		// get status information
		inline size_t getUndoIndex() const { return undoIndex; }
		inline bool canUndo() const { return undoIndex > 0; }
		inline bool canRedo() const { return undoIndex < size(); }

	private:
		size_t undoIndex = 0;
	} transactions;

	// details about bracketed text
	struct Bracket {
		Bracket(ImWchar sc, Coordinate s, ImWchar ec, Coordinate e, int l) : startChar(sc), start(s), endChar(ec), end(e), level(l) {}
		ImWchar startChar;
		Coordinate start;
		ImWchar endChar;
		Coordinate end;
		int level;

		inline bool isValid() const { return start.isValid() && end.isValid(); }
		inline bool isAfter(Coordinate location) const { return start > location; }
		inline bool isAround(Coordinate location) const { return start <= location && end >= location; }
	};

	class Brackets : public std::vector<Bracket> {
	public:
		// update the list of bracket pairs in the documenht
		void update(const Document& document);

		// manage active brackets
		void markActiveBrackets(Coordinate location);
		inline bool hasActiveBrackets() const { return active.size() != 0; }
		const Bracket& getClosetBracket() const { return at(active.back()); };
		inline void eachCompleteBracket(std::function<void(const Bracket&)> callback) const { for (auto& bracket : *this) { if (bracket.isValid()) callback(bracket); }; }
		inline void eachIncompleteBracket(std::function<void(const Bracket&)> callback) const { for (auto& bracket : *this) { if (!bracket.isValid()) callback(bracket); }; }
		inline void eachActiveBracket(std::function<void(const Bracket&)> callback) const { for (auto a : active) { callback(at(a)); }; }

	private:
		std::vector<size_t> active;
	} brackets;

	// set the editor's text
	void setText(const std::string& text);

	// render (parts of) the text editor
	void render(const char* title, const ImVec2& size, bool border);
	void renderSelections();
	void renderMatchingBrackets(bool documentHasChanged, bool mainCursorHasChanged);
	void renderText();
	void renderCursors();
	void renderLineNumbers();

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
	void addNextOccurrence();
	void selectAllOccurrences();

	void replaceTextInCurrentCursor(const std::string& text);
	void replaceTextInAllCursors(const std::string& text);

	// cursor/selection functions
	void moveUp(int lines, bool select);
	void moveDown(int lines, bool select);
	void moveLeft(bool select, bool wordMode);
	void moveRight(bool select, bool wordMode);
	void moveToTop(bool select);
	void moveToBottom(bool select);
	void moveToStartOfLine(bool select);
	void moveToEndOfLine(bool select);
	void moveTo(Coordinate coordinate, bool select);

	// add/delete characters
	void handleCharacter(ImWchar character);
	void handleBackspace(bool wordMode);
	void handleDelete(bool wordMode);

	// add/delete lines
	void removeSelectedLines();
	void insertLineAbove();
	void insertLineBelow();

	// transform selected lines
	void indentLines();
	void deindentLines();
	void moveUpLines();
	void moveDownLines();
	void toggleComments();

	// transform entire document
	void stripTrailingWhitespaces();
	void filterLines(std::function<std::string(std::string)> filter);
	void tabsToSpaces();
	void spacesToTabs();

	// transaction functions
	// note that strings must be UTF-8 encoded
	std::shared_ptr<Transaction> startTransaction();
	bool endTransaction(std::shared_ptr<Transaction> transaction);

	void insertTextIntoAllCursors(std::shared_ptr<Transaction> transaction, const std::string& text);
	void deleteTextFromAllCursors(std::shared_ptr<Transaction> transaction);
	void autoIndentAllCursors(std::shared_ptr<Transaction> transaction);
	Coordinate insertText(std::shared_ptr<Transaction> transaction, Coordinate start, const std::string& text);
	void deleteText(std::shared_ptr<Transaction> transaction, Coordinate start, Coordinate end);

	// utility functions
	static inline bool isBracketOpener(ImWchar ch) { return ch == '{' || ch == '[' || ch == '('; }
	static inline bool isBracketCloser(ImWchar ch) { return ch == '}' || ch == ']' || ch == ')'; }
	static inline ImWchar toBracketCloser(ImWchar ch) { return ch == '{' ? '}' : (ch == '[' ? ']' : (ch == '(' ? ')' : ch)); }
	static inline ImWchar toBracketOpener(ImWchar ch) { return ch == '}' ? '{' : (ch == ']' ? '[' : (ch == ')' ? '(' : ch)); }

	// editor options
	float lineSpacing = 1.0f;
	bool readOnly = false;
	bool autoIndent = true;
	bool showWhitespaces = true;
	bool showLineNumbers = true;
	bool showMatchingBrackets = true;
	bool completePairedGlyphs = false;

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
	ImWchar completePairCloser = 0;
	Coordinate completePairLocation;

	// color palette support
	inline ImU32 getColor(Color color) { return palette[static_cast<size_t>(color)]; }
	void updatePalette();

	static Palette defaultPalette;
	Palette paletteBase;
	Palette palette;
	float paletteAlpha;
};
