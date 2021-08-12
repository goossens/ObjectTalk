//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "component.h"


//
//	OtCompositeClass
//

class OtCompositeClass;
typedef std::shared_ptr<OtCompositeClass> OtComposite;

class OtCompositeClass : public OtComponentClass {
public:
	// add a new child
	OtObject add(OtObject child);

	// clear component tree
	void clear();

	// ensure specified component is allowed as a child
	virtual void validateChild(OtComponent child) {}

	// update component state
	void update();

	// render content
	void render();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtComposite create();

protected:
	std::vector<OtComponent> children;
};
