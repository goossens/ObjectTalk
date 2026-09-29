//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtBackgroundComponent.h"
#include "OtComponentObject.h"


//
//	OtBackgroundComponentObjectClass
//

class OtBackgroundComponentObjectClass;
using OtBackgroundComponentObject = OtObjectPointer<OtBackgroundComponentObjectClass>;

class OtBackgroundComponentObjectClass : public OtComponentObjectClass {
public:
	// access properties
	declareProperty(glm::vec3, color, Color)

	// get type definition
	inline static OtType getMeta()  {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtBackgroundComponentObjectClass>("BackgroundComponent", OtComponentObjectClass::getMeta());
			metaProperty(Background, Color)
		}

		return type;
	}

protected:
	// constructor
	friend class OtObjectPointer<OtBackgroundComponentObjectClass>;
	OtBackgroundComponentObjectClass() = default;
	OtBackgroundComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtBackgroundComponent& component() { return OtComponentObjectClass::component<OtBackgroundComponent>(scene, entity); }
};
