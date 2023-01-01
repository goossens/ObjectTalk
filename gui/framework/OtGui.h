//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtObject.h"


//
//	Forward references
//

class OtAppClass;


//
//	OtGui
//

class OtGuiClass;
typedef std::shared_ptr<OtGuiClass> OtGui;

class OtGuiClass : public OtObjectClass {
public:
	// register GUI module
	static void registerModule();

	// register app instance
	static void registerApp(OtAppClass* app);
	static void unregisterApp();
	static OtAppClass* getRegisteredApp() { return app; }

	// get type definition
	static OtType getMeta();

private:
	// registered app;
	static OtAppClass* app;
};
