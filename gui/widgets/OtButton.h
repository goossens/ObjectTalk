//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
//	OtButtonClass
//

class OtButtonClass;
using OtButton = OtObjectPointer<OtButtonClass>;

class OtButtonClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// access properties
	inline OtObject setLabel(const std::string& l) {
		label = l;
		return OtWidget(this);
	}

	inline OtObject setCallback(OtObject cb) {
		OtCallbackValidate(cb, 0);
		callback = cb;
		return OtWidget(this);
	}

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtButtonClass>;
	OtButtonClass() = default;

private:
	std::string label;
	OtObject callback;
};
