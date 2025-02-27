//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "TextEditor.h"

#include "OtDebugState.h"

#include "OtConsole.h"
#include "OtSubProcess.h"


//
//	OtDebuggerUI
//

class OtDebuggerUI {
public:
	// update debugger state
	void update(const std::string& message);

	// render the debug UI
	void render(OtSubProcess& subprocess, OtConsole& console);

private:
	// private rendering functions
	void renderControlBar(OtSubProcess& subprocess);
	void renderSourceCode();
	void renderDecorations(TextEditor::Decorator& decorator);
	void renderVariables();
	void renderVariable(OtDebugState::Variable& variable);

	// properties
	std::string currentDebuggable;
	size_t currentLine = 0;
	OtDebugState debugState;
	TextEditor sourceCode;

	// debugger layout parameters
	void determinePanelHeights();
	void determinePanelWidths();
	float contentsHeight = -1.0f;
	float contentsMinHeight;
	float contentsMaxHeight;
	float sourceCodeWidth = -1.0f;
	float sourceCodeMinWidth;
	float sourceCodeMaxWidth;
};
