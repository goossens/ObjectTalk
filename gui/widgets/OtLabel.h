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

#include "OtUi.h"

#include "OtWidget.h"


//
//	OtLabelClass
//

class OtLabelClass;
using OtLabel = OtObjectPointer<OtLabelClass>;

class OtLabelClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// access properties
	inline OtObject setLabel(const std::string& l) {
		label = l;
		return OtWidget(this);
	}

	inline OtObject setHorizontalAlignment(OtUi::Alignment alignment) {
		horizontalAlign = alignment;
		return OtWidget(this);
	}

	inline OtObject setVerticalAlignment(OtUi::Alignment alignment) {
		verticalAlign = alignment;
		return OtWidget(this);
	}

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtLabelClass>;
	OtLabelClass() = default;

private:
	// properties
	std::string label;
	OtUi::Alignment horizontalAlign = OtUi::Alignment::left;
	OtUi::Alignment verticalAlign = OtUi::Alignment::top;
};
