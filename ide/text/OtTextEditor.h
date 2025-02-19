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

#include "OtEditor.h"


//
//	OtTextEditor
//

class OtTextEditor : public OtEditor {
public:
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

	// visual text editor
	TextEditor editor;

	// properties
	size_t version = 0;
};
