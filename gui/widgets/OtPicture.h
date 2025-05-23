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

#include "OtTextureAsset.h"
#include "OtUi.h"

#include "OtWidget.h"


//
//	OtPictureClass
//

class OtPictureClass;
using OtPicture = OtObjectPointer<OtPictureClass>;

class OtPictureClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// access properties
	inline OtObject setPicture(const std::string& path) {
		picture = path;
		return OtWidget(this);
	}

	inline OtObject setScale(float s) {
		scale = s;
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
	friend class OtObjectPointer<OtPictureClass>;
	OtPictureClass() = default;

private:
	// properties
	OtAsset<OtTextureAsset> picture;
	float scale = 1.0f;
	OtUi::Alignment horizontalAlign = OtUi::Alignment::left;
	OtUi::Alignment verticalAlign = OtUi::Alignment::top;
};
