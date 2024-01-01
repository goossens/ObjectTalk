//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtObject.h"

#include "OtTransformComponent.h"
#include "OtScene.h"


//
//	OtTransformComponentObjectClass
//

class OtTransformComponentObjectClass;
using OtTransformComponentObject = OtObjectPointer<OtTransformComponentObjectClass>;

class OtTransformComponentObjectClass : public OtObjectClass {
public:
	// constructor
	OtTransformComponentObjectClass() = default;
	OtTransformComponentObjectClass(OtTransformComponent* component) : transform(component) {}

	// access transform parts
	void setTranslation(OtObject object);
	void setRotation(OtObject object);
	void setScale(OtObject object);

	OtObject getTranslation();
	OtObject getRotation();
	OtObject getScale();

	// get type definition
	static OtType getMeta();

private:
	// connection to component
	OtTransformComponent* transform;
};
