//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtObject.h"

#include "OtInstancingComponent.h"
#include "OtScene.h"


//
//	OtInstancingComponentObjectClass
//

class OtInstancingComponentObjectClass;
using OtInstancingComponentObject = OtObjectPointer<OtInstancingComponentObjectClass>;

class OtInstancingComponentObjectClass : public OtObjectClass {
public:
	// constructor
	OtInstancingComponentObjectClass() = default;
	OtInstancingComponentObjectClass(OtInstancingComponent* component) : instancing(component) {}

	// manipuliate instances
	void clearInstances() { instancing->clearInstances(); }
	void appendInstance(OtObject translation, OtObject rotation, OtObject scale);
	OtObject getInstance(size_t index);
	void setInstance(size_t index, OtObject translation, OtObject rotation, OtObject scale);
	void eraseInstance(size_t index);
	size_t instanceCount() { return instancing->instanceCount(); }

	// get type definition
	static OtType getMeta();

private:
	// connection to component
	OtInstancingComponent* instancing;
};
