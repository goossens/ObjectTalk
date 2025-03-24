//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtWidget.h"


//
//	OtTabClass
//

class OtTabClass;
using OtTab = OtObjectPointer<OtTabClass>;

class OtTabClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// access properties
	inline OtObject setLabel(const std::string& l) {
		label = l;
		return OtWidget(this);
	}

	// get maximum number of children
	inline int getMaxChildren() override { return 1; }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtTabClass>;
	OtTabClass() = default;

private:
	// properties
	std::string label;
};
