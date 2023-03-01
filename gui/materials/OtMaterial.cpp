//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"

#include "OtMaterial.h"


//
//	OtMaterialClass::getMeta
//

OtType OtMaterialClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtMaterialClass>("Material", OtGuiClass::getMeta());

		type->set("setFrontSide", OtFunction::create(&OtMaterialClass::setFrontSide));
		type->set("setBackSide", OtFunction::create(&OtMaterialClass::setBackSide));
		type->set("setDoubleSided", OtFunction::create(&OtMaterialClass::setDoubleSided));
		type->set("isFrontSided", OtFunction::create(&OtMaterialClass::isFrontSided));
		type->set("isBackSided", OtFunction::create(&OtMaterialClass::isBackSided));
		type->set("isDoubleSided", OtFunction::create(&OtMaterialClass::isDoubleSided));

		type->set("setTransparent", OtFunction::create(&OtMaterialClass::setTransparent));
		type->set("setOpaque", OtFunction::create(&OtMaterialClass::setOpaque));
		type->set("isTransparent", OtFunction::create(&OtMaterialClass::isTransparent));
		type->set("isOpaque", OtFunction::create(&OtMaterialClass::isOpaque));
	}

	return type;
}
