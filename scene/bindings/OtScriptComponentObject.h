//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtObject.h"

#include "OtEntityObject.h"
#include "OtScriptComponent.h"


//
//	OtScriptComponentObjectClass
//

class OtScriptComponentObjectClass;
using OtScriptComponentObject = OtObjectPointer<OtScriptComponentObjectClass>;

class OtScriptComponentObjectClass : public OtObjectClass {
public:
	// get scene and entity
	OtObject getScene();
	OtObject getEntity();

	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtScriptComponentObjectClass>;
	OtScriptComponentObjectClass() = default;

	OtScriptComponentObjectClass(OtScene* scene, OtEntity entity, OtScriptComponent* script) :
		scene(scene), entity(entity), script(script) {}

private:
	// connection to component
	friend class OtScriptComponent;
	OtScene* scene;
	OtEntity entity;
	OtScriptComponent* script;

	// function to stop direct instantiation
	static void invalidConstructor(size_t count, OtObject* parameters);
};
