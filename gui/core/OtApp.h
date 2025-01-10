//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFrameworkApp.h"
#include "OtWidget.h"


//
//	OtAppClass
//

class OtAppClass;
using OtApp = OtObjectPointer<OtAppClass>;

class OtAppClass : public OtWidgetClass, public OtFrameworkApp {
public:
	// get maximum number of children
	inline int getMaxChildren() override { return 1; }

	// lifetime events
	void onSetup() override;
	void onRender() override;
	void onTerminate() override;

	// run the app
	void run();

	// get type definition
	static OtType getMeta();
};
