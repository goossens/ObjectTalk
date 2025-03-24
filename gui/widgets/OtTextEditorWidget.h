//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
	inline int getTabSize() { return editor.GetTabSize(); }
	inline void setLineSpacing(float value) { editor.SetLineSpacing(value); }
	inline float getLineSpacing() { return editor.GetLineSpacing(); }
	inline void setReadOnlyEnabled(bool value) { editor.SetReadOnlyEnabled(value); }
	inline bool isReadOnlyEnabled() { return editor.IsReadOnlyEnabled(); }
	inline void setAutoIndentEnabled(bool value) { editor.SetAutoIndentEnabled(value); }
	inline bool isAutoIndentEnabled() { return editor.IsAutoIndentEnabled(); }
	inline void setShowWhitespacesEnabled(bool value) { editor.SetShowWhitespacesEnabled(value); }
	inline bool isShowWhitespacesEnabled() { return editor.IsShowWhitespacesEnabled(); }
	inline void setShowLineNumbersEnabled(bool value) { editor.SetShowLineNumbersEnabled(value); }
	inline bool isShowLineNumbersEnabled() { return editor.IsShowLineNumbersEnabled(); }
	inline void setShowScrollbarMiniMapEnabled(bool value) { editor.SetShowScrollbarMiniMapEnabled(value); }
	inline bool isShowScrollbarMiniMapEnabled() { return editor.IsShowScrollbarMiniMapEnabled(); }
	inline void setShowMatchingBrackets(bool value) { editor.SetShowMatchingBrackets(value); }
	inline bool isShowingMatchingBrackets() { return editor.IsShowingMatchingBrackets(); }
	inline void setCompletePairedGlyphs(bool value) { editor.SetCompletePairedGlyphs(value);  }
	inline bool isCompletingPairedGlyphs() { return editor.IsCompletingPairedGlyphs(); }
	inline void setOverwriteEnabled(bool value) { editor.SetOverwriteEnabled(value); }
	inline bool isOverwriteEnabled() { return editor.IsOverwriteEnabled(); }

	void setLanguage(const std::string& text);
	inline std::string getLanguage() { return editor.GetLanguageName(); }

	// access text (using UTF-8 encoded strings)
	inline void setText(const std::string& text) { editor.SetText(text); }
	inline std::string getText() { return editor.GetText(); }
	inline bool isEmpty() { return editor.IsEmpty(); }
	inline int lineCount() { return editor.GetLineCount(); }

	//	clipboard functions
	inline void cut() { editor.Cut(); }
	inline void copy() { editor.Copy(); }
	inline void paste() { editor.Paste(); }
	inline void undo() { editor.Undo(); }
	inline void redo() { editor.Redo(); }
	inline bool canUndo() { return editor.CanUndo(); };
	inline bool canRedo() { return  editor.CanRedo(); };
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
