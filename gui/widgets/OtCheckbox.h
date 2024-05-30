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

#include "OtCallback.h"
#include "OtWidget.h"


//
//	OtCheckboxClass
//

class OtCheckboxClass;
using OtCheckbox = OtObjectPointer<OtCheckboxClass>;

class OtCheckboxClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// access properties
	inline OtObject setLabel(const std::string& l) {
		label = l;
		return OtWidget(this);
	}

	inline OtObject setChecked(bool c) {
		checked = c;
		return OtWidget(this);
	}

	inline OtObject setCallback(OtObject cb) {
		OtCallbackValidate(cb, 1);
		callback = cb;
		return OtWidget(this);
	}

	inline bool isChecked() { return checked; }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

private:
	std::string label;
	bool checked = false;
	OtObject callback;
};
