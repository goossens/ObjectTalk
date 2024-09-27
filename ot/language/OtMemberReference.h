//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
#include "OtModule.h"
#include "OtReference.h"
#include "OtIdentifier.h"


//
//	OtMemberReference
//

class OtMemberReferenceClass;
using OtMemberReference = OtObjectPointer<OtMemberReferenceClass>;

class OtMemberReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtMemberReferenceClass() = default;
	OtMemberReferenceClass(OtObject o, size_t m) : object(o), member(m) {}

	// debugging support
	std::string describe() override { return object->getType()->getName() + " " + std::string(OtIdentifier::name(member)); }

	// (de)reference functions
	OtObject deref() { return resolveMember(object, member); }
	OtObject assign(OtObject value) { return object->set(member, value); }

	// resolve member reference and deal with bound functions if required
	static OtObject resolveMember(OtObject& object, size_t member) {
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
	size_t getMember() { return member; }

	// get type definition
	static OtType getMeta();

private:
	OtObject object;
	size_t member;
};
