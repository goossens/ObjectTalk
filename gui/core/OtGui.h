//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtObject.h"
#include "OtValue.h"

#include "OtUi.h"


//
//	OtGui
//

class OtGuiClass;
using OtGui = OtObjectPointer<OtGuiClass>;

class OtGuiClass : public OtObjectClass {
public:
	// get type definition
	static OtType getMeta();
};


//
//	Allow OtUiAlignment as ObjectTalk value
//


template <>
struct OtValue<OtUiAlignment> {
	static inline OtObject encode(OtUiAlignment value) { return OtInteger::create(int(value)); }
	static inline OtUiAlignment decode(OtObject object) { return OtUiAlignment(object->operator int()); }
};
