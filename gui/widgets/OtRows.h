//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "OtWidget.h"


//
//	OtRowsClass
//

class OtRowsClass;
using OtRows = OtObjectPointer<OtRowsClass>;

class OtRowsClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// set properties
	OtObject setHeights(OtObject heights);
	OtObject setBorders(bool b);
	OtObject setResizable(bool r);

	// get maximum number of children
	inline int getMaxChildren() override { return -1; }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtRowsClass>;
	OtRowsClass() = default;

private:
	// properties
	std::vector<float> logicalHeights;
	bool borders = true;
	bool resizable = true;
};
