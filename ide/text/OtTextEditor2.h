//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtTextWidget.h"

#include "OtEditor.h"


//
//	OtTextEditor2
//

//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtEditor.h"
#include "OtTextWidget.h"


//
//	OtTextEditor2
//

class OtTextEditor2 : public OtEditor {
public:
	// constructor
	OtTextEditor2();

	// get editor status
	inline bool isDirty() override { return editor.GetUndoIndex() != version; }

	// render the parts
	void renderMenu(bool canRun) override;
	void renderEditor() override;

protected:
	// file access functions
	void clear() override;
	void load() override;
	void save() override;

	// visual text editor
	OtTextWidget editor;
	bool focusOnEditor = true;

	// find and replace support
	bool findReplaceVisible = false;
	bool focusOnFind = false;
	std::string findText;
	std::string replaceText;
	bool caseSensitiveFind = false;
	bool wholeWordFind = false;

	void openFindReplace();
	void find();
	void findNext();
	void findAll();
	void replace();
	void replaceAll();

	// properties
	size_t version = 0;
	int scrollToLine = 1;
};
