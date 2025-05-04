//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"

#include "OtMathModule.h"

#include "OtMaterialComponentObject.h"


//
//	OtMaterialComponentObjectClass::getMeta
//

OtType OtMaterialComponentObjectClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtMaterialComponentObjectClass>("MaterialComponent", OtObjectClass::getMeta());

		type->set("setAlbedo", OtFunction::create(&OtMaterialComponentObjectClass::setAlbedo));
		type->set("setMetallic", OtFunction::create(&OtMaterialComponentObjectClass::setMetallic));
		type->set("setRoughness", OtFunction::create(&OtMaterialComponentObjectClass::setRoughness));
		type->set("setEmissive", OtFunction::create(&OtMaterialComponentObjectClass::setEmissive));
		type->set("setAo", OtFunction::create(&OtMaterialComponentObjectClass::setAo));
		type->set("setTextureOffset", OtFunction::create(&OtMaterialComponentObjectClass::setTextureOffset));
		type->set("setTextureScale", OtFunction::create(&OtMaterialComponentObjectClass::setTextureScale));

		type->set("setAlbedoTexture", OtFunction::create(&OtMaterialComponentObjectClass::setAlbedoTexture));
		type->set("setNormalTexture", OtFunction::create(&OtMaterialComponentObjectClass::setNormalTexture));
		type->set("setMetallicRoughnessTexture", OtFunction::create(&OtMaterialComponentObjectClass::setMetallicRoughnessTexture));
		type->set("setEmissiveTexture", OtFunction::create(&OtMaterialComponentObjectClass::setEmissiveTexture));
		type->set("setAoTexture", OtFunction::create(&OtMaterialComponentObjectClass::setAoTexture));
	}

	return type;
}
