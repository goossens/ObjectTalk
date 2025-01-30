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

#include "OtUi.h"

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

	// access properties
	inline void setText(const std::string& text) { editor.SetText(text); }
	inline std::string getText() { return editor.GetText(); }

	void setLanguage(const std::string& text);
	inline std::string getLanguage() { return editor.GetLanguageName(); }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

private:
	// properties
	TextEditor editor;
};
