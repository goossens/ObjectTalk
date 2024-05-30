//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtWidget.h"


//
//	OtHsplitClass
//

class OtHsplitClass;
using OtHsplit = OtObjectPointer<OtHsplitClass>;

class OtHsplitClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// set location and limits
	inline OtObject setSplit(float s) {
		split = s;
		return OtWidget(this);
	}

	inline OtObject setSplitLimits(float mn, float mx) {
		minSplit = mn;
		maxSplit = mx;
		return OtWidget(this);
	}

	// get maximum number of children
	inline int getMaxChildren() override { return 2; }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

private:
	// properties
	float split = 0.5f;
	float minSplit = 0.25f;
	float maxSplit = 0.75f;
};
