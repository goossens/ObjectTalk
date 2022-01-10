//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "TextEditor.h"

#include "ide.h"


//
//	OtEditorClass
//

class OtEditorClass;
typedef std::shared_ptr<OtEditorClass> OtEditor;

class OtEditorClass : public OtIdeClass {
public:
	// render the editor
	void render();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtEditor create();
	static OtEditor create(const std::string& filename);

private:
	// the file we are editing
	std::string filename;

	// text edit state
	TextEditor editor;
};
