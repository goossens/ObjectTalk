//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtWidget.h"


//
//	OtPropertiesClass
//

class OtPropertiesClass;
using OtProperties = OtObjectPointer<OtPropertiesClass>;

class OtPropertiesClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// access properties
	inline OtObject setSplit(const float s) {
		split = s;
		return OtWidget(this);
	}

	// get maximum number of children
	inline int getMaxChildren() override { return -1; }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtPropertiesClass>;
	OtPropertiesClass() = default;

private:
	// propertoes
	float split = 0.5f;
};
