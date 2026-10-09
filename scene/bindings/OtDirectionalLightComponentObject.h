//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtDirectionalLightComponent.h"
#include "OtComponentObject.h"


//
//	OtDirectionalLightComponentObjectClass
//

class OtDirectionalLightComponentObjectClass;
using OtDirectionalLightComponentObject = OtObjectPointer<OtDirectionalLightComponentObjectClass>;

class OtDirectionalLightComponentObjectClass : public OtComponentObjectClass {
public:
	// access properties
	declareProperty(float, elevation, Elevation)
	declareProperty(float, azimuth, Azimuth)
	declareVectorProperty(3, color, Color)
	declareProperty(float, ambient, Ambient)
	declareProperty(float, castShadow, CastShadow)

	// get type definition
	static inline OtType getMeta()  {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtDirectionalLightComponentObjectClass>("DirectionalLightComponent", OtComponentObjectClass::getMeta());
			metaProperty(DirectionalLight, Elevation)
			metaProperty(DirectionalLight, Azimuth)
			metaProperty(DirectionalLight, Color)
			metaProperty(DirectionalLight, Ambient)
			metaProperty(DirectionalLight, CastShadow)
		}

		return type;
	}

protected:
	// constructor
	friend class OtObjectPointer<OtDirectionalLightComponentObjectClass>;
	OtDirectionalLightComponentObjectClass() = default;
	OtDirectionalLightComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtDirectionalLightComponent& component() { return OtComponentObjectClass::component<OtDirectionalLightComponent>(scene, entity); }
};
