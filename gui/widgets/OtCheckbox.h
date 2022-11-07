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
//	OtCheckboxClass
//

class OtCheckboxClass;
typedef std::shared_ptr<OtCheckboxClass> OtCheckbox;

class OtCheckboxClass : public OtWidgetClass {
public:
	// initialize checkbox
	void init(const std::string& label, bool checked, OtObject callback);

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCheckbox create();

private:
	// properties
	std::string label;
	OtObject callback;
	bool checked = false;
};
