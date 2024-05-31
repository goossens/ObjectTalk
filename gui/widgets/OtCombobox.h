//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <vector>

#include "OtCallback.h"
#include "OtWidget.h"


//
//	OtComboboxClass
//

class OtComboboxClass;
using OtCombobox = OtObjectPointer<OtComboboxClass>;

class OtComboboxClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// access properties
	inline OtObject setLabel(const std::string& l) {
		label = l;
		return OtWidget(this);
	}

	inline OtObject setValue(const std::string& v) {
		value = v;
		return OtWidget(this);
	}

	OtObject setOptions(OtObject options);

	inline OtObject setCallback(OtObject cb) {
		OtCallbackValidate(cb, 1);
		callback = cb;
		return OtWidget(this);
	}

	inline std::string getValue() { return value; }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

private:
	std::string label;
	std::string value;
	std::vector<std::string> options;
	OtObject callback;
};
