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
//	OtTreeNodeClass
//

class OtTreeNodeClass;
using OtTreeNode = OtObjectPointer<OtTreeNodeClass>;

class OtTreeNodeClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// adjust properties
	OtObject setTitle(const std::string& t) { title = t; return OtObject(this); }
	std::string getTitle() { return title; }

	OtObject setOpen(bool o) { open = 0; return OtObject(this); }
	bool getOpen() { return open; }

	// ensure specified node is allowed as a child
	void validateChild(OtNode child) override;

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

private:
	// properties
	std::string title;
	bool open = true;
};
