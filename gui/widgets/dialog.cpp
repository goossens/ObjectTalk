//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
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
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtDialogClass>("Dialog", OtScreenObjectClass::getMeta());
	}

	return type;
}
