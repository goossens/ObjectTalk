//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtPointLightComponent.h"
#include "OtComponentObject.h"


//
//	OtPointLightComponentObjectClass
//

class OtPointLightComponentObjectClass;
using OtPointLightComponentObject = OtObjectPointer<OtPointLightComponentObjectClass>;

class OtPointLightComponentObjectClass : public OtComponentObjectClass {
public:
	// access properties
	declareVectorProperty(3, offset, Offset)
	declareProperty(float, radius, Radius)
	declareVectorProperty(3, color, Color)

	// get type definition
	static inline OtType getMeta()  {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtPointLightComponentObjectClass>("PointLightComponent", OtComponentObjectClass::getMeta());
			metaProperty(PointLight, Offset)
			metaProperty(PointLight, Radius)
			metaProperty(PointLight, Color)
		}

		return type;
	}

protected:
	// constructor
	friend class OtObjectPointer<OtPointLightComponentObjectClass>;
	OtPointLightComponentObjectClass() = default;
	OtPointLightComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtPointLightComponent& component() { return OtComponentObjectClass::component<OtPointLightComponent>(scene, entity); }
};
