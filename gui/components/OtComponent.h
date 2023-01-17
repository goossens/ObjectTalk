//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	OtComponent
//

class OtComponent {
public:
	// destructor
	virtual ~OtComponent() {}

	// GUI to change component properties
	virtual void renderGUI() {}
};
