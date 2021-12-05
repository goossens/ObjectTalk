//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "widget.h"


//
//	OtDialogClass
//

class OtDialogClass;
typedef std::shared_ptr<OtDialogClass> OtDialog;

class OtDialogClass : public OtWidgetClass {
public:
	// get type definition
	static OtType getMeta();

	// create a new object
	static OtDialog create();
};
