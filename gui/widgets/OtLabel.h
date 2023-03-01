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
//	OtLabelClass
//

class OtLabelClass;
using OtLabel = OtObjectPointer<OtLabelClass>;

class OtLabelClass : public OtWidgetClass {
public:
	// initialize label
	void init(const std::string& label);

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

private:
	// properties
	std::string label;
};
