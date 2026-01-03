//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <string>

#include "TextEditor.h"
#include "TextDiff.h"

#include "OtEditor.h"


//
//	OtTextEditor
//

class OtTextEditor : public OtEditor {
public:
	// constructor
	OtTextEditor();

	// get editor status
	inline bool isDirty() override { return editor.GetUndoIndex() != version; }

	// render the parts
	void renderMenus() override;
	void handleShortcuts() override;
	void renderEditor() override;

protected:
	// file access functions
	void clear() override;
	void load() override;
	void save() override;

	void showDiff();

	// visual text editor
	TextEditor editor;
	TextDiff diff;
	std::string originalText;

	float fontSize = 17.0f;
	inline void zoomIn() { fontSize = std::clamp(fontSize + 1.0f, 8.0f, 24.0f); }
	inline void zoomOut() { fontSize = std::clamp(fontSize - 1.0f, 8.0f, 24.0f); }

	// properties
	size_t version = 0;
};
