//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtAppObject.h"


//
//	OtWindowClass
//

class OtWindowClass;
typedef std::shared_ptr<OtWindowClass> OtWindow;

class OtWindowClass : public OtAppObjectClass {
public:
	// initialize window
	void init(const std::string& t, int w, int h, int f) { title = t; width = w; height = h; flags = f; }

	// adjust window properties
	OtObject setTitle(const std::string& t) { title = t; return shared(); }
	std::string getTitle() { return title; }

	OtObject setSize(int w, int h) { width = w; height = h; return shared(); }
	int getWidth() { return width; }
	int getHeight() { return height; }

	OtObject setFlags(int f) { flags = f; return shared(); }
	int getFlags() { return flags; }

	// ensure specified node is allowed as a child
	void validateChild(OtNode child) override;

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtWindow create();

private:
	std::string title;
	int width = 600;
	int height = 300;
	int flags = 0;
};
