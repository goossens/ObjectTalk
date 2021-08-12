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
	// set new parent
    void setParent(OtComponent p=nullptr) { parent = p; }
	OtComponent getParent() { return parent; }

	// clear component tree
	virtual void clear() { parent = nullptr; }

	// update enabled flag
	void enable() { enabled = true; }
	void disable() { enabled = false; }
	void setEnabled(bool e) { enabled = e; }
	bool isEnabled() { return enabled; }

	// update state (called every frame so be carefull)
    virtual void update() {}

    // render content
    virtual void render() {}

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtComponent create();

protected:
	// parent component
	OtComponent parent = nullptr;

	// enabled flag
	bool enabled = true;
};
