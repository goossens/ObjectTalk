//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "OtCallback.h"

#include "OtCanvas.h"
#include "OtFrameBuffer.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtUi.h"

#include "OtWidget.h"


//
//	OtCanvasStackClass
//

class OtCanvasStackClass;
using OtCanvasStack = OtObjectPointer<OtCanvasStackClass>;

class OtCanvasStackClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// access properties
	inline OtObject setSize(float w, float h) { width = w; height = h; return OtWidget(this); }
	inline OtObject setHorizontalAlignment(OtUi::Alignment alignment) { horizontalAlign = alignment; return OtWidget(this); }
	inline OtObject setVerticalAlignment(OtUi::Alignment alignment) { verticalAlign = alignment; return OtWidget(this); }

	// manage layers
	int addCanvas(OtObject canvas);
	void deleteCanvas(int id);
	void enableCanvas(int id);
	void disableCanvas(int id);
	void rerenderCanvas(int id);

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

private:
	// properties
	float width = 100.0f;
	float height = 100.0f;
	OtUi::Alignment horizontalAlign = OtUi::Alignment::left;
	OtUi::Alignment verticalAlign = OtUi::Alignment::top;

	// canvas layers
	struct Canvas {
		// constructor
		Canvas(int i, OtObject c) : id(i), canvas(c) {}

		// properties
		int id;
		OtObject canvas;
		OtFrameBuffer framebuffer{OtTexture::rgba8Texture, OtTexture::d24s8Texture};
	};

	std::vector<Canvas> canvases;
	int layerID = 1;

	// find specified canvas
	Canvas* findCanvas(int id);

	// GPU resource
	OtFrameBuffer framebuffer{OtTexture::rgba8Texture};
	OtSampler sampler{"s_texture", OtTexture::pointSampling | OtTexture::clampSampling};
	OtShaderProgram program = OtShaderProgram("OtFilterVS", "OtAlphaFS");
};
