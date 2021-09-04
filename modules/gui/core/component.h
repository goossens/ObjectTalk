//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "gui.h"


//
//	OtComponentClass
//

class OtComponentClass;
typedef std::shared_ptr<OtComponentClass> OtComponent;

class OtComponentClass : public OtGuiClass {
public:
	// add / remove components
	OtObject add(OtObject object);
	OtObject remove(OtObject object);

	// ensure specified component is allowed as a child
	virtual void validateChild(OtComponent child);

	// clear component tree
	void clear();

	// update enabled flag
	OtObject enable() { enabled = true; return shared(); }
	OtObject disable() { enabled = false; return shared(); }
	OtObject setEnabled(bool e) { enabled = e; return shared(); }
	bool isEnabled() { return enabled; }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtComponent create();

protected:
	// parent component
	OtComponent parent = nullptr;

	// our children
	std::vector<OtComponent> children;

	// enabled flag
	bool enabled = true;
};