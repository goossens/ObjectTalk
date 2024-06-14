//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtTextureAsset.h"
#include "OtUi.h"

#include "OtWidget.h"


//
//	OtFilmStripClass
//

class OtFilmStripClass;
using OtFilmStrip = OtObjectPointer<OtFilmStripClass>;

class OtFilmStripClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* paraFilmStrips);

	// access properties
	inline OtObject setTexture(const std::string& path) {
		texture = path;
		return OtWidget(this);
	}

	inline OtObject setValue(float v) {
		value = v;
		return OtWidget(this);
	}

	inline float getValue() { return value; }

	inline OtObject setHorizontalAlignment(OtUiAlignment alignment) {
		horizontalAlign = alignment;
		return OtWidget(this);
	}

	inline OtObject setVerticalAlignment(OtUiAlignment alignment) {
		verticalAlign = alignment;
		return OtWidget(this);
	}

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

private:
	// properties
	OtAsset<OtTextureAsset> texture;
	OtUiAlignment horizontalAlign = OtUiAlignLeft;
	OtUiAlignment verticalAlign = OtUiAlignTop;
	float value = 0.0f;
};
