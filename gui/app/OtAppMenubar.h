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
//	OtAppMenubarClass
//

class OtAppMenubarClass;
typedef std::shared_ptr<OtAppMenubarClass> OtAppMenubar;

class OtAppMenubarClass : public OtAppObjectClass {
public:
	// ensure specified node is allowed as a child
	void validateChild(OtNode child) override;

	// what is our height?
	size_t getHeight() { return height; }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtAppMenubar create();

private:
	size_t height;
};
