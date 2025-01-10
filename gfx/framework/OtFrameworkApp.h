//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	OtFrameworkApp class
//

class OtFrameworkApp {
public:
	// destructor
	virtual inline ~OtFrameworkApp() {}

	// event handlers
	virtual inline void onSetup() {}
	virtual inline void onRender() {}
	virtual inline void onTerminate() {}
	virtual inline bool onCanQuit() { return true; }
};
