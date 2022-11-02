//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtGui.h"


//
//	OtComponentClass
//

class OtComponentClass;
typedef std::shared_ptr<OtComponentClass> OtComponent;
typedef std::weak_ptr<OtComponentClass> OtComponentWeak;

class OtComponentClass : public OtGuiClass {
public:
	// add / remove child components
	OtObject add(OtObject object);
	OtObject remove(OtObject object);

	// get our parent
	OtComponent getParent() { return parent.lock(); }
	bool hasParent() { return !parent.expired(); }

	// ensure specified component is allowed as a child
	virtual void validateChild(OtComponent child);

	// remove all children
	void clear();

	// return number of children
	size_t size() { return children.size(); }

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
	OtComponentWeak parent;

	// our children
	std::vector<OtComponent> children;

	// enabled flag
	bool enabled = true;
};
