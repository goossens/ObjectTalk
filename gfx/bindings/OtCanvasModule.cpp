//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nanovg.h"

#include "OtClass.h"
#include "OtInteger.h"
#include "OtModule.h"

#include "OtCanvasModule.h"


//
//	OtCanvasClass::getMeta
//

OtType OtCanvasClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtCanvasClass>("Canvas", OtObjectClass::getMeta());

		type->set("compositeOperation", OtFunction::create(&OtCanvasClass::compositeOperation));
		type->set("compositeBlendFunc", OtFunction::create(&OtCanvasClass::compositeBlendFunc));
		type->set("compositeBlendFuncSeparate", OtFunction::create(&OtCanvasClass::compositeBlendFuncSeparate));

		type->set("save", OtFunction::create(&OtCanvasClass::save));
		type->set("restore", OtFunction::create(&OtCanvasClass::restore));
		type->set("reset", OtFunction::create(&OtCanvasClass::reset));

		type->set("loadImage", OtFunction::create(&OtCanvasClass::loadImage));
		type->set("deleteImage", OtFunction::create(&OtCanvasClass::deleteImage));
		type->set("getImageWidth", OtFunction::create(&OtCanvasClass::getImageWidth));
		type->set("getImageHeight", OtFunction::create(&OtCanvasClass::getImageHeight));

		type->set("createLinearGradient", OtFunction::create(&OtCanvasClass::createLinearGradient));
		type->set("createBoxGradient", OtFunction::create(&OtCanvasClass::createBoxGradient));
		type->set("createRadialGradient", OtFunction::create(&OtCanvasClass::createRadialGradient));
		type->set("createTexturePattern", OtFunction::create(&OtCanvasClass::createTexturePattern));
		type->set("deletePaint", OtFunction::create(&OtCanvasClass::deletePaint));

		type->set("loadFont", OtFunction::create(&OtCanvasClass::loadFont));

		type->set("antiAlias", OtFunction::create(&OtCanvasClass::antiAlias));
		type->set("strokeColor", OtFunction::create(&OtCanvasClass::strokeColor));
		type->set("strokePaint", OtFunction::create(&OtCanvasClass::strokePaint));
		type->set("fillColor", OtFunction::create(&OtCanvasClass::fillColor));
		type->set("fillPaint", OtFunction::create(&OtCanvasClass::fillPaint));
		type->set("miterLimit", OtFunction::create(&OtCanvasClass::miterLimit));
		type->set("strokeWidth", OtFunction::create(&OtCanvasClass::strokeWidth));
		type->set("lineCap", OtFunction::create(&OtCanvasClass::lineCap));
		type->set("lineJoin", OtFunction::create(&OtCanvasClass::lineJoin));
		type->set("globalAlpha", OtFunction::create(&OtCanvasClass::globalAlpha));

		type->set("translate", OtFunction::create(&OtCanvasClass::translate));
		type->set("rotate", OtFunction::create(&OtCanvasClass::rotate));
		type->set("scale", OtFunction::create(&OtCanvasClass::scale));
		type->set("skewX", OtFunction::create(&OtCanvasClass::skewX));
		type->set("skewY", OtFunction::create(&OtCanvasClass::skewY));
		type->set("resetTransform", OtFunction::create(&OtCanvasClass::resetTransform));

		type->set("beginPath", OtFunction::create(&OtCanvasClass::beginPath));
		type->set("moveTo", OtFunction::create(&OtCanvasClass::moveTo));
		type->set("lineTo", OtFunction::create(&OtCanvasClass::lineTo));
		type->set("bezierCurveTo", OtFunction::create(&OtCanvasClass::bezierCurveTo));
		type->set("quadraticCurveTo", OtFunction::create(&OtCanvasClass::quadraticCurveTo));
		type->set("arcTo", OtFunction::create(&OtCanvasClass::arcTo));
		type->set("closePath", OtFunction::create(&OtCanvasClass::closePath));
		type->set("pathWinding", OtFunction::create(&OtCanvasClass::pathWinding));

		type->set("arc", OtFunction::create(&OtCanvasClass::arc));
		type->set("rect", OtFunction::create(&OtCanvasClass::rect));
		type->set("roundedRect", OtFunction::create(&OtCanvasClass::roundedRect));
		type->set("ellipse", OtFunction::create(&OtCanvasClass::ellipse));
		type->set("circle", OtFunction::create(&OtCanvasClass::circle));

		type->set("stroke", OtFunction::create(&OtCanvasClass::stroke));
		type->set("fill", OtFunction::create(&OtCanvasClass::fill));

		type->set("drawImage", OtFunction::create(&OtCanvasClass::drawImage));

		type->set("fontFace", OtFunction::create(&OtCanvasClass::fontFace));
		type->set("fontSize", OtFunction::create(&OtCanvasClass::fontSize));
		type->set("fontBlur", OtFunction::create(&OtCanvasClass::fontBlur));
		type->set("fontLetterSpacing", OtFunction::create(&OtCanvasClass::fontLetterSpacing));
		type->set("fontLineHeight", OtFunction::create(&OtCanvasClass::fontLineHeight));
		type->set("fontAlign", OtFunction::create(&OtCanvasClass::fontAlign));
		type->set("text", OtFunction::create(&OtCanvasClass::text));
		type->set("textBox", OtFunction::create(&OtCanvasClass::textBox));

		type->set("textSize", OtFunction::create(&OtCanvasClass::textSize));
		type->set("textBoxSize", OtFunction::create(&OtCanvasClass::textBoxSize));

		type->set("getWidth", OtFunction::create(&OtCanvasClass::getWidth));
		type->set("getHeight", OtFunction::create(&OtCanvasClass::getHeight));

		type->set("enable", OtFunction::create(&OtCanvasClass::enable));
		type->set("disable", OtFunction::create(&OtCanvasClass::disable));
		type->set("isEnabled", OtFunction::create(&OtCanvasClass::isEnabled));

		type->set("requestRerender", OtFunction::create(&OtCanvasClass::requestRerender));
		type->set("markClean", OtFunction::create(&OtCanvasClass::markClean));
		type->set("needsRerender", OtFunction::create(&OtCanvasClass::needsRerender));
	}

	return type;
}


//
//	RegisterEnums
//

static void RegisterEnums(OtModule& module) {
	module->set("repeatX", OtInteger::create(NVG_IMAGE_REPEATX));
	module->set("repeatY", OtInteger::create(NVG_IMAGE_REPEATY));
	module->set("flipY", OtInteger::create(NVG_IMAGE_FLIPY));
	module->set("nearest", OtInteger::create(NVG_IMAGE_NEAREST));

	module->set("sourceOver", OtInteger::create(NVG_SOURCE_OVER));
	module->set("sourceIn", OtInteger::create(NVG_SOURCE_IN));
	module->set("sourceOut", OtInteger::create(NVG_SOURCE_OUT));
	module->set("sourceAtop", OtInteger::create(NVG_ATOP));
	module->set("destinationOver", OtInteger::create(NVG_DESTINATION_OVER));
	module->set("destinationIn", OtInteger::create(NVG_DESTINATION_IN));
	module->set("destinationOut", OtInteger::create(NVG_DESTINATION_OUT));
	module->set("destinationAtop", OtInteger::create(NVG_DESTINATION_ATOP));
	module->set("lighter", OtInteger::create(NVG_LIGHTER));
	module->set("copy", OtInteger::create(NVG_COPY));
	module->set("xor", OtInteger::create(NVG_XOR));

	module->set("zero", OtInteger::create(NVG_ZERO));
	module->set("one", OtInteger::create(NVG_ONE));
	module->set("sourceColor", OtInteger::create(NVG_SRC_COLOR));
	module->set("oneMinusSourceColor", OtInteger::create(NVG_ONE_MINUS_SRC_COLOR));
	module->set("destinationColor", OtInteger::create(NVG_DST_COLOR));
	module->set("oneMinusDestinationColor", OtInteger::create(NVG_ONE_MINUS_DST_COLOR));
	module->set("sourceAlpha", OtInteger::create(NVG_SRC_ALPHA));
	module->set("oneMinusSourceAlpha", OtInteger::create(NVG_ONE_MINUS_SRC_ALPHA));
	module->set("destinationAlpha", OtInteger::create(NVG_DST_ALPHA));
	module->set("oneMinusDestinationAlpha", OtInteger::create(NVG_ONE_MINUS_DST_ALPHA));
	module->set("alphaSaturate", OtInteger::create(NVG_SRC_ALPHA_SATURATE));

	module->set("buttCap", OtInteger::create(NVG_BUTT));
	module->set("roundCap", OtInteger::create(NVG_ROUND));
	module->set("squareCap", OtInteger::create(NVG_SQUARE));

	module->set("miterJoin", OtInteger::create(NVG_MITER));
	module->set("roundJoin", OtInteger::create(NVG_ROUND));
	module->set("bevelJoin", OtInteger::create(NVG_BEVEL));

	module->set("ccw", OtInteger::create(NVG_CCW));
	module->set("cw", OtInteger::create(NVG_CW));

	module->set("left", OtInteger::create(NVG_ALIGN_LEFT));
	module->set("center", OtInteger::create(NVG_ALIGN_CENTER));
	module->set("right", OtInteger::create(NVG_ALIGN_RIGHT));

	module->set("top", OtInteger::create(NVG_ALIGN_TOP));
	module->set("middle", OtInteger::create(NVG_ALIGN_MIDDLE));
	module->set("bottom", OtInteger::create(NVG_ALIGN_BOTTOM));
	module->set("baseline", OtInteger::create(NVG_ALIGN_BASELINE));
}


//
//	Module registration
//

static OtModuleRegistration registration{"canvas", [](OtModule module) {
	// register enums
	RegisterEnums(module);

	// register classes
	module->set("Canvas", OtClass::create(OtCanvasClass::getMeta()));
}};
