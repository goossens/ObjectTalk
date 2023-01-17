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
//	OtPanelClass
//

class OtPanelClass;
typedef std::shared_ptr<OtPanelClass> OtPanel;

class OtPanelClass : public OtAppObjectClass {
public:
	// initialize panel
	void init(size_t count, OtObject* parameters);

	// update properties
	OtObject setScreenArea(int x, int y, int w, int h);

	// ensure specified node is allowed as a child
	void validateChild(OtNode child) override;

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtPanel create();

private:
	int x = 0;
	int y = 0;
	int w = 30;
	int h = 100;
};
