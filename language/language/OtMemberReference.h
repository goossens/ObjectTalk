//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtBoundFunction.h"
#include "OtByteCodeFunction.h"
#include "OtFunction.h"
#include "OtGlobal.h"
#include "OtIdentifier.h"
#include "OtModule.h"
#include "OtReference.h"


//
//	OtMemberReference
//

class OtMemberReferenceClass;
using OtMemberReference = OtObjectPointer<OtMemberReferenceClass>;

class OtMemberReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtMemberReferenceClass() = default;
	OtMemberReferenceClass(OtObject o, OtID m) : object(o), member(m) {}

	// debugging support
	inline std::string describe() override { return object->getType()->getName() + " " + std::string(OtIdentifier::name(member)); }

	// (de)reference functions
	inline OtObject deref() { return resolveMember(object, member); }
	inline OtObject assign(OtObject value) { return object->set(member, value); }

	// resolve member reference and deal with bound functions if required
	static inline OtObject resolveMember(OtObject& object, OtID member) {
		// get the member
		auto memberObject = object->get(member);

		// never create bound functions for Modules or Globals
		if (object.isKindOf<OtModuleClass>() || object.isKindOf<OtGlobalClass>()) {
			return memberObject;

		// create bound function if required
		} else if (memberObject.isKindOf<OtFunctionClass>() || memberObject.isKindOf<OtByteCodeFunctionClass>()) {
			return OtBoundFunction::create(object, memberObject);

		// it's just a member variable
		} else {
			return memberObject;
		}
	}

	// access properties
	OtObject& getObject() { return object; }
	OtID getMember() { return member; }

	// get type definition
	static OtType getMeta();

private:
	OtObject object;
	OtID member;
};
