//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"

#include "OtAppObject.h"
#include "OtWidget.h"


//
//	OtAppObjectClass::validateChild
//

void OtAppObjectClass::validateChild(OtNode child) {
	OtExcept("A [%s] can't have children", getType()->getName().c_str());
}


//
//	OtAppObjectClass::render
//

void OtAppObjectClass::render() {
	// render all children
	for (auto& child : children) {
		if (child->isEnabled()) {
			OtWidget(child)->render();
		}
	}
}


//
//	OtAppObjectClass::getMeta
//

OtType OtAppObjectClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtAppObjectClass>("AppObject", OtNodeClass::getMeta());
	}

	return type;
}
