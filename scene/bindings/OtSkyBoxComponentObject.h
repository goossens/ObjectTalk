//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtArray.h"
#include "OtString.h"

#include "OtComponentObject.h"
#include "OtSkyBoxComponent.h"


//
//	OtSkyBoxComponentObjectClass
//

class OtSkyBoxComponentObjectClass;
using OtSkyBoxComponentObject = OtObjectPointer<OtSkyBoxComponentObjectClass>;

class OtSkyBoxComponentObjectClass : public OtComponentObjectClass {
public:
	// access properties
	declareAssetProperty(asset, Asset)

	// get type definition
	static inline OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtSkyBoxComponentObjectClass>("SkyBoxComponent", OtComponentObjectClass::getMeta());
			metaProperty(SkyBox, Asset)
		}

		return type;
	}

protected:
	// constructor
	friend class OtObjectPointer<OtSkyBoxComponentObjectClass>;
	OtSkyBoxComponentObjectClass() = default;
	OtSkyBoxComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtSkyBoxComponent& component() { return OtComponentObjectClass::component<OtSkyBoxComponent>(scene, entity); }
};
