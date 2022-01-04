//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "widget.h"


//
//	OtTreeNodeClass
//

class OtTreeNodeClass;
typedef std::shared_ptr<OtTreeNodeClass> OtTreeNode;

class OtTreeNodeClass : public OtWidgetClass {
public:
	// initialize
	void init(const std::string& label);

	// ensure specified component is allowed as a child
	void validateChild(OtComponent child);

	// render content
	void render();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtTreeNode create();

private:
	// properties
	std::string label;
};
