//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "guidialog.h"


//
//	OtDialogClass::getMeta
//

OtType OtDialogClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtDialogClass>("Dialog", OtWidgetClass::getMeta());
	}

	return type;
}


//
//	OtDialogClass::create
//

OtDialog OtDialogClass::create() {
	OtDialog dialog = std::make_shared<OtDialogClass>();
	dialog->setType(getMeta());
	return dialog;
}
