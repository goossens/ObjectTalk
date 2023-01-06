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

		type->set("setFrontSide", OtFunctionClass::create(&OtMaterialClass::setFrontSide));
		type->set("setBackSide", OtFunctionClass::create(&OtMaterialClass::setBackSide));
		type->set("setDoubleSided", OtFunctionClass::create(&OtMaterialClass::setDoubleSided));
		type->set("isFrontSided", OtFunctionClass::create(&OtMaterialClass::isFrontSided));
		type->set("isBackSided", OtFunctionClass::create(&OtMaterialClass::isBackSided));
		type->set("isDoubleSided", OtFunctionClass::create(&OtMaterialClass::isDoubleSided));

		type->set("setTransparent", OtFunctionClass::create(&OtMaterialClass::setTransparent));
		type->set("setOpaque", OtFunctionClass::create(&OtMaterialClass::setOpaque));
		type->set("isTransparent", OtFunctionClass::create(&OtMaterialClass::isTransparent));
		type->set("isOpaque", OtFunctionClass::create(&OtMaterialClass::isOpaque));
	}

	return type;
}
