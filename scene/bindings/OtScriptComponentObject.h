//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtComponentObject.h"
#include "OtScriptComponent.h"


//
//	OtScriptComponentObjectClass
//

class OtScriptComponentObjectClass;
using OtScriptComponentObject = OtObjectPointer<OtScriptComponentObjectClass>;

class OtScriptComponentObjectClass : public OtComponentObjectClass {
public:
	// get type definition
	static inline OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtScriptComponentObjectClass>("ScriptComponent", OtComponentObjectClass::getMeta());
		}

		return type;
	}

protected:
	// return class name
	inline const char* getClassName() override { return "ScriptComponent"; }

	// constructor
	friend class OtObjectPointer<OtScriptComponentObjectClass>;
	OtScriptComponentObjectClass() = default;
	OtScriptComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// connection to component
	friend class OtScriptComponent;

	// get component reference
	inline OtScriptComponent& component() { return OtComponentObjectClass::component<OtScriptComponent>(scene, entity); }
};
