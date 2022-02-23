//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtAppObject.h"


//
//	OtDialogClass
//

class OtDialogClass;
typedef std::shared_ptr<OtDialogClass> OtDialog;

class OtDialogClass : public OtAppObjectClass {
public:
	// get type definition
	static OtType getMeta();
};
