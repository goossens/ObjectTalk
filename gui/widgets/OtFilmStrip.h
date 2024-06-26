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
	void init(size_t count, OtObject* parameters);

	// access properties
	inline OtObject setFilmStrip(const std::string& path) {
		filmstrip = path;
		return OtWidget(this);
	}

	inline OtObject setFrames(const int f) {
		frames = f;
		return OtWidget(this);
	}

	inline OtObject setHorizontal() {
		horizontal = true;
		return OtWidget(this);
	}

	inline OtObject setScale(float s) {
		scale = s;
		return OtWidget(this);
	}

	inline OtObject setCallback(OtObject cb) {
		OtCallbackValidate(cb, 1);
		callback = cb;
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
	OtAsset<OtTextureAsset> filmstrip;
	int frames = 0;
	bool horizontal = false;
	float scale = 1.0f;
	OtUiAlignment horizontalAlign = OtUiAlignLeft;
	OtUiAlignment verticalAlign = OtUiAlignTop;
	float value = 0.0f;
	OtObject callback;
};
