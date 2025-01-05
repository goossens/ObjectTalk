#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#ifdef IMGUI_EDITOR_NO_BOOST
	#include <regex>
	namespace boost = std;
#else
	#include <boost/regex.hpp>
#endif

#include "imgui.h"

class IMGUI_API TextEditor
{
public:
	// ------------- Exposed API ------------- //

	TextEditor();

	// access editor options
	inline void SetReadOnlyEnabled(bool aValue) { mReadOnly = aValue; }
	inline bool IsReadOnlyEnabled() const { return mReadOnly; }
	inline void SetAutoIndentEnabled(bool aValue) { mAutoIndent = aValue; }
	inline bool IsAutoIndentEnabled() const { return mAutoIndent; }
	inline void SetShowWhitespacesEnabled(bool aValue) { mShowWhitespaces = aValue; }
	inline bool IsShowWhitespacesEnabled() const { return mShowWhitespaces; }
	inline void SetShowLineNumbersEnabled(bool aValue) { mShowLineNumbers = aValue; }
	inline bool IsShowLineNumbersEnabled() const { return mShowLineNumbers; }
	inline void SetShortTabsEnabled(bool aValue) { mShortTabs = aValue; }
	inline bool IsShortTabsEnabled() const { return mShortTabs; }
	inline void SetShowMatchingBrackets(bool aValue) { mShowMatchingBrackets = aValue; }
	inline bool IsShowingMatchingBrackets() const { return mShowMatchingBrackets; }
	inline void SetCompletePairedGlyphs(bool aValue) { mCompletePairedGlyphs = aValue; }
	inline bool IsCompletingPairedGlyphs() const { return mCompletePairedGlyphs; }
	inline bool IsOverwriteEnabled() const { return mOverwrite; }
	inline void SetTabSize(int aValue) { mTabSize = std::max(1, std::min(8, aValue)); }
	inline int GetTabSize() const { return mTabSize; }
	inline void SetLineSpacing(float aValue) { mLineSpacing = std::max(1.0f, std::min(2.0f, aValue)); }
	inline float GetLineSpacing() const { return mLineSpacing; }

	// access editor color palette
	enum class PaletteIndex
	{
		Default,
		Keyword,
		Number,
		String,
		CharLiteral,
		Punctuation,
		Preprocessor,
		Identifier,
		KnownIdentifier,
		PreprocIdentifier,
		Comment,
		MultiLineComment,
		Background,
		Cursor,
		Selection,
		ErrorMarker,
		ControlCharacter,
		Breakpoint,
		LineNumber,
		CurrentLineNumber,
		Max
	};

	typedef std::array<ImU32, static_cast<size_t>(PaletteIndex::Max)> Palette;

	inline void SetPalette(const Palette& aValue) { mPaletteBase = aValue; mPaletteAlpha = -1.0f; }
	inline const Palette& GetPalette() const { return mPaletteBase; }
	inline static void SetDefaultPalette(const Palette& aValue) { defaultPalette = aValue; }
	inline static Palette& GetDefaultPalette() { return defaultPalette; }

	static const Palette& GetDarkPalette();
	static const Palette& GetLightPalette();
	static const Palette& GetRetroBluePalette();
	static const Palette& GetMarianaPalette();

	// access editor color laguage definition
	struct LanguageDefinition
	{
		static const LanguageDefinition& Cpp();
		static const LanguageDefinition& Hlsl();
		static const LanguageDefinition& Glsl();
		static const LanguageDefinition& Python();
		static const LanguageDefinition& C();
		static const LanguageDefinition& Sql();
		static const LanguageDefinition& AngelScript();
		static const LanguageDefinition& Lua();
		static const LanguageDefinition& Cs();
		static const LanguageDefinition& Json();
		static const LanguageDefinition& Markdown();

		typedef std::pair<std::string, PaletteIndex> TokenRegexString;
		typedef bool(*TokenizeCallback)(const char* in_begin, const char* in_end, const char*& out_begin, const char*& out_end, PaletteIndex& paletteIndex);

		std::string mName;
		std::unordered_set<std::string> mKeywords;
		std::unordered_set<std::string> mIdentifiers;
		std::unordered_set<std::string> mPreprocIdentifiers;
		std::string mCommentStart, mCommentEnd, mSingleLineComment;
		char mPreprocChar = '#';
		TokenizeCallback mTokenize = nullptr;
		std::vector<TokenRegexString> mTokenRegexStrings;
		bool mCaseSensitive = true;
	};

	void SetLanguageDefinition(const LanguageDefinition& aLanguageDef);
	inline const LanguageDefinition& GetLanguageDefinition() const { return *mLanguageDefinition; };
	inline std::string GetLanguageDefinitionName() const { return mLanguageDefinition != nullptr ? mLanguageDefinition->mName : "None"; }

	// manipulate selections
	void SelectAll();
	void SelectLine(int aLine);
	void SelectLines(int aStartLine, int aEndLine);
	bool AnyCursorHasSelection() const;
	bool AllCursorsHaveSelection() const;
	void ClearCursors();

	// find/replace support
	void SelectFirstOccurrenceOf(const std::string& aText, bool aCaseSensitive = true, bool aWholeWord = false);
	void SelectNextOccurrenceOf(const std::string& aText, bool aCaseSensitive = true, bool aWholeWord = false);
	void SelectAllOccurrencesOf(const std::string& aText, bool aCaseSensitive = true, bool aWholeWord = false);
	void ReplaceTextInCurrentCursor(const std::string& aText);
	void ReplaceTextInAllCursors(const std::string& aText);

	// access visible lines
	inline int GetFirstVisibleLine() const { return mFirstVisibleLine; }
	inline int GetLastVisibleLine() const { return mLastVisibleLine; }

	enum class SetViewAtLineMode { FirstVisibleLine, Centered, LastVisibleLine };
	void SetViewAtLine(int aLine, SetViewAtLineMode aMode, bool aSetCursor = false);

	// clipboard actions
	void Copy();
	void Cut();
	void Paste();
	void Undo(int aSteps = 1);
	void Redo(int aSteps = 1);
	inline bool CanUndo() const { return !mReadOnly && mUndoIndex > 0; };
	inline bool CanRedo() const { return !mReadOnly && mUndoIndex < mUndoBuffer.size(); };
	inline size_t GetUndoIndex() const { return mUndoIndex; };

	// access editor context
	void SetText(const std::string& aText);
	std::string GetText() const;

	void SetTextLines(const std::vector<std::string>& aLines);
	std::vector<std::string> GetTextLines() const;

	inline bool IsEmpty() const {  return mLines.size() == 1 && mLines[0].size() == 0; }
	inline int GetLineCount() const {  return static_cast<int>(mLines.size()); }

	// access error markers
	inline void SetErrorMarkers(const std::map<int, std::string>& aMarkers) { mErrorMarkers = aMarkers; }
	inline void ClearErrorMarkers() { mErrorMarkers.clear(); }
	inline bool HasErrorMarkers() const { return mErrorMarkers.size() != 0; }

	// useful editor functions
	void StripTrailingWhitespaces();
	void FilterLines(std::function<std::string(std::string)> filter);
	void TabsToSpaces();
	void SpacesToTabs();

	// render the editor in a Dear ImGui context
	void Render(const char* aTitle, const ImVec2& aSize = ImVec2(), bool aBorder = false);

private:
	// ------------- Internal ------------- //

	// Represents a character coordinate from the user's point of view,
	// i. e. consider an uniform grid (assuming fixed-width font) on the
	// screen as it is rendered, and each cell has its own coordinate, starting from 0.
	// Tabs are counted as [1..mTabSize] count empty spaces, depending on
	// how many space is necessary to reach the next tab stop.
	// For example, coordinate (1, 5) represents the character 'B' in a line "\tABC", when mTabSize = 4,
	// because it is rendered as "    ABC" on the screen.
	struct Coordinates
	{
		Coordinates() = default;
		Coordinates(int aLine, int aColumn) : mLine(aLine), mColumn(aColumn) {}

		inline bool operator ==(const Coordinates& o) const { return mLine == o.mLine && mColumn == o.mColumn; }
		inline bool operator !=(const Coordinates& o) const { return mLine != o.mLine || mColumn != o.mColumn; }
		inline bool operator <(const Coordinates& o) const { return mLine != o.mLine ? mLine < o.mLine : mColumn < o.mColumn; }
		inline bool operator >(const Coordinates& o) const { return mLine != o.mLine ? mLine > o.mLine : mColumn > o.mColumn; }
		inline bool operator <=(const Coordinates& o) const { return mLine != o.mLine ? mLine < o.mLine : mColumn <= o.mColumn; }
		inline bool operator >=(const Coordinates& o) const { return mLine != o.mLine ? mLine > o.mLine : mColumn >= o.mColumn; }

		inline Coordinates operator -(const Coordinates& o) { return Coordinates(mLine - o.mLine, mColumn - o.mColumn); }
		inline Coordinates operator +(const Coordinates& o) { return Coordinates(mLine + o.mLine, mColumn + o.mColumn); }

		int mLine = 0;
		int mColumn = 0;
	};

	struct Cursor
	{
		inline Coordinates GetSelectionStart() const { return mInteractiveStart < mInteractiveEnd ? mInteractiveStart : mInteractiveEnd; }
		inline Coordinates GetSelectionEnd() const { return mInteractiveStart > mInteractiveEnd ? mInteractiveStart : mInteractiveEnd; }
		inline bool HasSelection() const { return mInteractiveStart != mInteractiveEnd; }

		Coordinates mInteractiveStart = {0, 0};
		Coordinates mInteractiveEnd = {0, 0};
	};

	struct State
	{
		void AddCursor();
		int GetLastAddedCursorIndex();
		void SortCursorsFromTopToBottom();

		int mCurrentCursor = 0;
		int mLastAddedCursor = 0;
		std::vector<Cursor> mCursors = {{{0,0}}};
	};

	struct Glyph
	{
		Glyph(char aChar, PaletteIndex aColorIndex) : mChar(aChar), mColorIndex(aColorIndex),
			mComment(false), mMultiLineComment(false), mPreprocessor(false) {}

		char mChar;
		PaletteIndex mColorIndex = PaletteIndex::Default;
		bool mComment : 1;
		bool mMultiLineComment : 1;
		bool mPreprocessor : 1;
	};

	typedef std::vector<Glyph> Line;

	struct UndoOperation
	{
		enum class Type
		{
			Add,
			Delete
		};

		UndoOperation() = default;
		UndoOperation(const std::string& aText, Coordinates aStart, Coordinates aEnd, Type aType)
			: mText(aText), mStart(aStart), mEnd(aEnd), mType(aType) {}

		std::string mText;
		Coordinates mStart;
		Coordinates mEnd;
		Type mType;
	};

	class UndoRecord
	{
	public:
		UndoRecord() = default;
		UndoRecord(const std::vector<UndoOperation>& aOperations, State& aBefore, State& aAfter);

		void Undo(TextEditor* aEditor);
		void Redo(TextEditor* aEditor);

		std::vector<UndoOperation> mOperations;

		State mBefore;
		State mAfter;
	};

	typedef std::vector<std::pair<boost::regex, PaletteIndex>> RegexList;

	std::string GetText(const Coordinates& aStart, const Coordinates& aEnd) const;
	std::string GetAllSelectedText() const;
	std::string GetSelectedText(int aCursor = -1) const;

	void SetCursorPosition(const Coordinates& aPosition, int aCursor = -1, bool aClearSelection = true);

	int InsertTextAt(Coordinates& aWhere, const char* aValue);
	void InsertTextAtCursor(const char* aValue, int aCursor = -1);
	void InsertLineBelow();
	void InsertLineAbove();

	enum class MoveDirection
	{
		Right,
		Left,
		Up,
		Down
	};

	bool Move(int& aLine, int& aCharIndex, bool aLeft = false, bool aLockLine = false) const;
	void MoveCharIndexAndColumn(int aLine, int& aCharIndex, int& aColumn) const;
	void MoveCoords(Coordinates& aCoords, MoveDirection aDirection, bool aWordMode = false, int aLineCount = 1) const;

	void MoveUp(int aAmount = 1, bool aSelect = false);
	void MoveDown(int aAmount = 1, bool aSelect = false);
	void MoveLeft(bool aSelect = false, bool aWordMode = false);
	void MoveRight(bool aSelect = false, bool aWordMode = false);
	void MoveTop(bool aSelect = false);
	void MoveBottom(bool aSelect = false);
	void MoveHome(bool aSelect = false);
	void MoveEnd(bool aSelect = false);

	void EnterCharacter(ImWchar aChar);
	void Backspace(bool aWordMode = false);
	void Delete(bool aWordMode = false, const State* aEditorState = nullptr);

	void SetSelection(Coordinates aStart, Coordinates aEnd, int aCursor = -1);
	void SetSelection(int aStartLine, int aStartChar, int aEndLine, int aEndChar, int aCursor = -1);

	void SelectNextOccurrenceOf(const char* aText, int aTextSize, int aCursor = -1, bool aCaseSensitive = true, bool aWholeWord = false);
	void AddCursorForNextOccurrence(bool aCaseSensitive = true, bool aWholeWord = false);
	bool FindNextOccurrence(const char* aText, int aTextSize, const Coordinates& aFrom, Coordinates& outStart, Coordinates& outEnd, bool aCaseSensitive = true, bool aWholeWord = false);
	bool FindMatchingBracket(int aLine, int aCharIndex, Coordinates& out);

	void ChangeCurrentLinesIndentation(bool aIncrease);
	void MoveUpCurrentLines();
	void MoveDownCurrentLines();
	void ToggleLineComment();
	void RemoveCurrentLines();

	float TextDistanceToLineStart(const Coordinates& aFrom, bool aSanitizeCoords = true) const;
	void EnsureCursorVisible(int aCursor = -1, bool aStartToo = false);

	Coordinates SanitizeCoordinates(const Coordinates& aValue) const;
	Coordinates GetActualCursorCoordinates(int aCursor = -1, bool aStart = false) const;
	Coordinates ScreenPosToCoordinates(const ImVec2& aPosition, bool aInsertionMode = false, bool* isOverLineNumber = nullptr) const;
	Coordinates FindWordStart(const Coordinates& aFrom) const;
	Coordinates FindWordEnd(const Coordinates& aFrom) const;
	bool IsWholeWord(const Coordinates& aFrom, const Coordinates& aTo) const;
	int GetCharacterIndexL(const Coordinates& aCoordinates) const;
	int GetCharacterIndexR(const Coordinates& aCoordinates) const;
	int GetCharacterColumn(int aLine, int aIndex) const;
	int GetFirstVisibleCharacterIndex(int aLine) const;
	int GetLineMaxColumn(int aLine, int aLimit = -1) const;
	inline int GetLineMaxIndex(const Line& aLine) const { return static_cast<int>(aLine.size()); }

	Line& InsertLine(int aIndex);
	void RemoveLine(int aIndex, const std::unordered_set<int>* aHandledCursors = nullptr);
	void RemoveLines(int aStart, int aEnd);
	void DeleteRange(const Coordinates& aStart, const Coordinates& aEnd);
	void DeleteSelection(int aCursor = -1);

	void RemoveGlyphsFromLine(int aLine, int aStartChar, int aEndChar = -1);
	void AddGlyphsToLine(int aLine, int aTargetIndex, Line::iterator aSourceStart, Line::iterator aSourceEnd);
	void AddGlyphToLine(int aLine, int aTargetIndex, Glyph aGlyph);
	ImU32 GetGlyphColor(const Glyph& aGlyph) const;

	void HandleKeyboardInputs(bool aParentIsFocused = false);
	void HandleMouseInputs();
	void UpdateViewVariables(float aScrollX, float aScrollY);
	void Render();

	void OnCursorPositionChanged();
	void OnBeforeLineChange(int aLine, int aColumn, int aCharCount, bool aDeleted);
	void OnAfterLineChange();
	void MergeCursorsIfPossible();

	void AddUndo(UndoRecord& aValue);

	void Colorize(int aFromLine = 0, int aCount = -1);
	void ColorizeRange(int aFromLine = 0, int aToLine = 0);
	void ColorizeInternal();
	void UpdatePalette();
	inline ImU32 GetColor(PaletteIndex index) const { return mPalette[static_cast<size_t>(index)]; }

	inline bool IsHorizontalScrollbarVisible() const { return mCurrentSpaceWidth > mContentWidth; }
	inline bool IsVerticalScrollbarVisible() const { return mCurrentSpaceHeight > mContentHeight; }
	inline int TabSizeAtColumn(int aColumn) const { return mTabSize - (aColumn % mTabSize); }

	std::vector<Line> mLines;
	State mState;
	std::vector<UndoRecord> mUndoBuffer;
	size_t mUndoIndex = 0;

	int mTabSize = 4;
	float mLineSpacing = 1.0f;
	bool mOverwrite = false;
	bool mReadOnly = false;
	bool mAutoIndent = true;
	bool mShowWhitespaces = true;
	bool mShowLineNumbers = true;
	bool mShortTabs = false;
	bool mShowMatchingBrackets = true;
	bool mCompletePairedGlyphs = false;

	int mSetViewAtLine = -1;
	SetViewAtLineMode mSetViewAtLineMode;
	int mEnsureCursorVisible = -1;
	bool mEnsureCursorVisibleStartToo = false;

	float mTextStart = 20.0f; // position (in pixels) where a code line starts relative to the left of the TextEditor.
	int mLeftMargin = 10;
	ImVec2 mCharAdvance;
	float mCurrentSpaceHeight = 20.0f;
	float mCurrentSpaceWidth = 20.0f;
	float mLastClickTime = -1.0f;
	int mFirstVisibleLine = 0;
	int mLastVisibleLine = 0;
	int mVisibleLineCount = 0;
	int mFirstVisibleColumn = 0;
	int mLastVisibleColumn = 0;
	int mVisibleColumnCount = 0;
	float mContentWidth = 0.0f;
	float mContentHeight = 0.0f;
	float mScrollX = 0.0f;
	float mScrollY = 0.0f;
	bool mCursorPositionChanged = false;
	bool mCursorOnBracket = false;
	Coordinates mMatchingBracketCoords;
	float mCursorAnimationTimer = 0.0f;
	std::unordered_map<int, Coordinates> mCursorIndices;
	std::map<int, std::string> mErrorMarkers;

	int mColorRangeMin = 0;
	int mColorRangeMax = 0;
	bool mCheckComments = true;
	static Palette defaultPalette;
	Palette mPaletteBase;
	Palette mPalette;
	float mPaletteAlpha;
	const LanguageDefinition* mLanguageDefinition = nullptr;
	RegexList mRegexList;
};
