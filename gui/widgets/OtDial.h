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

#include "OtFrameBuffer.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtTextureAsset.h"
#include "OtUi.h"

#include "OtWidget.h"


//
//	OtDialClass
//

class OtDialClass;
using OtDial = OtObjectPointer<OtDialClass>;

class OtDialClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// access properties
	inline OtObject setBackground(const std::string& path, float x=0.0f, float y=0.0f) {
		background = path;
		bx = x;
		by = y;
		redraw = true;
		return OtWidget(this);
	}

	inline OtObject setNeedle(const std::string& path, float x=0.0f, float y=0.0f) {
		needle = path;
		nx = x;
		ny = y;
		redraw = true;
		return OtWidget(this);
	}

	inline OtObject setValue(float v) {
		redraw = value != v;
		value = v;
		return OtWidget(this);
	}

	inline float getValue() { return value; }

	inline OtObject setValueLimits(float mn, float mx) {
		minValue = mn;
		maxValue = mx;
		redraw = true;
		return OtWidget(this);
	}

	inline OtObject setRotationLimits(float mn, float mx) {
		minRotation = mn;
		maxRotation = mx;
		redraw = true;
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
	friend class OtObjectPointer<OtDialClass>;
	OtDialClass() = default;

private:
	// properties
	OtAsset<OtTextureAsset> background;
	OtAsset<OtTextureAsset> needle;
	float bx = 0.0f;
	float by = 0.0f;
	float nx = 0.0f;
	float ny = 0.0f;
	float scale = 1.0f;
	float minRotation = -90.0f;
	float maxRotation = 90.0f;
	OtUi::Alignment horizontalAlign = OtUi::Alignment::left;
	OtUi::Alignment verticalAlign = OtUi::Alignment::top;
	float minValue = 0.0f;
	float maxValue = 1.0f;
	float value = 0.0f;

	// work variables
	OtFrameBuffer framebuffer{OtTexture::rgba8Texture, OtTexture::noTexture, 1, true};
	OtSampler sampler{"s_texture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtShaderProgram program{"OtDialVS", "OtDialFS"};
	bool redraw = true;
};
