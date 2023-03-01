//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtWidget.h"


//
//	OtMenuClass
//

class OtMenuClass;
using OtMenu = OtObjectPointer<OtMenuClass>;

class OtMenuClass : public OtWidgetClass {
public:
	// initialize menu
	void init(const std::string& t) { title = t; }

	// adjust menu properties
	OtObject setTitle(const std::string& t) { title = t; return OtObject(this); }
	std::string getTitle() { return title; }

	// ensure specified node is allowed as a child
	void validateChild(OtNode child) override;

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

private:
	// menu properties
	std::string title = "Menu";
};
