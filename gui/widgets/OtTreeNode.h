//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtWidget.h"


//
//	OtTreeNodeClass
//

class OtTreeNodeClass;
typedef std::shared_ptr<OtTreeNodeClass> OtTreeNode;

class OtTreeNodeClass : public OtWidgetClass {
public:
	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// adjust properties
	OtObject setTitle(const std::string& t) { title = t; return shared(); }
	std::string getTitle() { return title; }

	OtObject setOpen(bool o) { open = 0; return shared(); }
	bool getOpen() { return open; }

	// ensure specified component is allowed as a child
	void validateChild(OtComponent child) override;

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtTreeNode create();

private:
	// properties
	std::string title;
	bool open = true;
};
