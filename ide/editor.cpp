//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "editor.h"


//
//	OtEditorClass::getMeta
//

OtType OtEditorClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtEditorClass>("Editor", OtIdeClass::getMeta());
	}

	return type;
}


//
//	OtEditorClass::create
//

OtEditor OtEditorClass::create() {
	OtEditor editor = std::make_shared<OtEditorClass>();
	editor->setType(getMeta());
	return editor;
}
