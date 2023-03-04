//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtHttpResponse.h"
#include "OtInternal.h"


//
// OtHttpNotFound
//

class OtHttpNotFoundClass;
using OtHttpNotFound = OtObjectPointer<OtHttpNotFoundClass>;

class OtHttpNotFoundClass : public OtInternalClass {
public:
	OtHttpNotFoundClass() = default;
	OtHttpNotFoundClass(OtHttpResponse s) : res(s) {}

	// execute not found
	void call() {
		// nobody wanted the request so it must be a case of "Resource Not Found"
		res->setStatus(404);
		res->end();
	}

	// get type definition
	static OtType getMeta() {
		static OtType type;

		if (!type) {
			type = OtType::create<OtHttpNotFoundClass>("HttpNotFound", OtInternalClass::getMeta());
			type->set("__call__", OtFunction::create(&OtHttpNotFoundClass::call));
		}

		return type;
	}

private:
	OtHttpResponse res;
};
