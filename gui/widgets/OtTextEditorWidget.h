//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "TextEditor.h"

#include "OtWidget.h"


//
//	OtTextEditorWidgetClass
//

class OtTextEditorWidgetClass;
using OtTextEditorWidget = OtObjectPointer<OtTextEditorWidgetClass>;

class OtTextEditorWidgetClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// access editor options
	inline void setTabSize(int value) { editor.SetTabSize(value); }
	inline size_t getTabSize() { return editor.GetTabSize(); }
	inline void setInsertSpacesOnTabs(bool value) { editor.SetInsertSpacesOnTabs(value); }
	inline bool isInsertSpacesOnTabs() { return editor.IsInsertSpacesOnTabs(); }
	inline void setLineSpacing(float value) { editor.SetLineSpacing(value); }
	inline float getLineSpacing() { return editor.GetLineSpacing(); }
	inline void setWordWrapEnabled(bool value) { editor.SetWordWrapEnabled(value); }
	inline bool isWordWrapEnabled() { return editor.IsWordWrapEnabled(); }
	inline void setReadOnlyEnabled(bool value) { editor.SetReadOnlyEnabled(value); }
	inline bool isReadOnlyEnabled() { return editor.IsReadOnlyEnabled(); }
	inline void setFindReplaceEnabled(bool value) { editor.SetFindReplaceEnabled(value); }
	inline bool isFindReplaceEnabled() { return editor.IsFindReplaceEnabled(); }
	inline void setCaretsVisible(bool value) { editor.SetCaretsVisible(value); }
	inline bool isCaretsVisible() { return editor.IsCaretsVisible(); }
	inline void setAutoIndentEnabled(bool value) { editor.SetAutoIndentEnabled(value); }
	inline bool isAutoIndentEnabled() { return editor.IsAutoIndentEnabled(); }
	inline void setShowWhitespacesEnabled(bool value) { editor.SetShowWhitespacesEnabled(value); }
	inline bool isShowWhitespacesEnabled() { return editor.IsShowWhitespacesEnabled(); }
	inline void setShowSpacesEnabled(bool value) { editor.SetShowSpacesEnabled(value); }
	inline bool isShowSpacesEnabled() { return editor.IsShowSpacesEnabled(); }
	inline void setShowTabsEnabled(bool value) { editor.SetShowTabsEnabled(value); }
	inline bool isShowTabsEnabled() { return editor.IsShowTabsEnabled(); }
	inline void setShowLineNumbersEnabled(bool value) { editor.SetShowLineNumbersEnabled(value); }
	inline bool isShowLineNumbersEnabled() { return editor.IsShowLineNumbersEnabled(); }
	inline void setShowMiniMapEnabled(bool value) { editor.SetShowMiniMapEnabled(value); }
	inline bool isShowMiniMapEnabled() { return editor.IsShowMiniMapEnabled(); }
	inline void setMiniMapColumns(size_t value) { editor.SetMiniMapColumns(value); }
	inline size_t getMiniMapColumns() { return editor.GetMiniMapColumns(); }
	inline void setShowScrollbarMiniMapEnabled(bool value) { editor.SetShowScrollbarMiniMapEnabled(value); }
	inline bool isShowScrollbarMiniMapEnabled() { return editor.IsShowScrollbarMiniMapEnabled(); }
	inline void setShowPanScrollIndicatorEnabled(bool value) { editor.SetShowPanScrollIndicatorEnabled(value); }
	inline bool isShowPanScrollIndicatorEnabled() { return editor.IsShowPanScrollIndicatorEnabled(); }
	inline void setShowCurrentLineHighlightEnabled(bool value) { editor.SetShowCurrentLineHighlightEnabled(value); }
	inline bool isShowCurrentLineHighlightEnabled() { return editor.IsShowCurrentLineHighlightEnabled(); }
	inline void setShowMatchingBrackets(bool value) { editor.SetShowMatchingBrackets(value); }
	inline bool isShowingMatchingBrackets() { return editor.IsShowingMatchingBrackets(); }
	inline void setCompletePairedGlyphs(bool value) { editor.SetCompletePairedGlyphs(value);  }
	inline bool isCompletingPairedGlyphs() { return editor.IsCompletingPairedGlyphs(); }
	inline void setLineFoldingEnabled(bool value) { editor.SetLineFoldingEnabled(value);  }
	inline bool isLineFoldingEnabled() { return editor.IsLineFoldingEnabled(); }
	inline void setOverwriteEnabled(bool value) { editor.SetOverwriteEnabled(value); }
	inline bool isOverwriteEnabled() { return editor.IsOverwriteEnabled(); }
	inline void setMiddleMousePanMode() { editor.SetMiddleMousePanMode(); }
	inline void setMiddleMouseScrollMode() { editor.SetMiddleMouseScrollMode(); }
	inline bool isMiddleMousePanMode() { return editor.IsMiddleMousePanMode(); }
	inline void setLineNumberLeftMargin(size_t value) { editor.SetLineNumberLeftMargin(value); }
	inline size_t getLineNumberLeftMargin() { return editor.GetLineNumberLeftMargin(); }
	inline void setDecorationLeftMargin(size_t value) { editor.SetDecorationLeftMargin(value); }
	inline size_t getDecorationLeftMargin() { return editor.GetDecorationLeftMargin(); }
	inline void setTextLeftMargin(size_t value) { editor.SetTextLeftMargin(value); }
	inline size_t getTextLeftMargin() { return editor.GetTextLeftMargin(); }

	void setLanguage(const std::string& text);
	inline std::string getLanguage() { return editor.GetLanguageName(); }

	// access text (using UTF-8 encoded strings)
	inline void setText(const std::string& text) { editor.SetText(text); }
	inline std::string getText() { return editor.GetText(); }
	inline bool isEmpty() { return editor.IsEmpty(); }
	inline size_t lineCount() { return editor.GetLineCount(); }

	//	clipboard functions
	inline void cut() { editor.Cut(); }
	inline void copy() { editor.Copy(); }
	inline void paste() { editor.Paste(); }
	inline void undo() { editor.Undo(); }
	inline void redo() { editor.Redo(); }
	inline bool canUndo() { return editor.CanUndo(); };
	inline bool canRedo() { return editor.CanRedo(); };
	inline size_t getUndoIndex() { return editor.GetUndoIndex(); }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtTextEditorWidgetClass>;
	OtTextEditorWidgetClass() = default;

private:
	// properties
	TextEditor editor;
};
