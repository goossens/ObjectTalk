//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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

#include "OtCanvas.h"


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

	module->set("ccwWinding", OtInteger::create(NVG_SOLID));
	module->set("cwWinding", OtInteger::create(NVG_HOLE));

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
