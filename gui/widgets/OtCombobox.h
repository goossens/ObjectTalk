//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
//	OtComboboxClass
//

class OtComboboxClass;
typedef std::shared_ptr<OtComboboxClass> OtCombobox;

class OtComboboxClass : public OtWidgetClass {
public:
	// initialize combobox
	void init(const std::string& title, OtObject values, OtObject selected, OtObject callback);

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCombobox create();

private:
	// properties
	std::string title;
	std::vector<std::string> values;
	std::string selected;
	OtObject callback;
};
