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
#include "OtMessageComponent.h"


//
//	OtMessageComponentObjectClass
//

class OtMessageComponentObjectClass;
using OtMessageComponentObject = OtObjectPointer<OtMessageComponentObjectClass>;

class OtMessageComponentObjectClass : public OtComponentObjectClass {
public:
	// manage message
	inline void show(const std::string& msg, float visible, float fade) { component().show(msg, visible, fade); }
	inline void hide() { component().hide(); }

	// get type definition
	static inline OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtMessageComponentObjectClass>("MessageComponent", OtComponentObjectClass::getMeta());
			type->set("show", OtFunction::create(&OtMessageComponentObjectClass::show));
			type->set("hide", OtFunction::create(&OtMessageComponentObjectClass::hide));
		}

		return type;
	}

protected:
	// constructor
	friend class OtObjectPointer<OtMessageComponentObjectClass>;
	OtMessageComponentObjectClass() = default;
	OtMessageComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtMessageComponent& component() { return OtComponentObjectClass::component<OtMessageComponent>(scene, entity); }
};
