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
typedef std::shared_ptr<OtHttpNotFoundClass> OtHttpNotFound;

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
			type = OtTypeClass::create<OtHttpNotFoundClass>("HttpNotFound", OtInternalClass::getMeta());
			type->set("__call__", OtFunctionClass::create(&OtHttpNotFoundClass::call));
		}

		return type;
	}

	// create a new object
	static OtHttpNotFound create(OtHttpResponse s) {
		OtHttpNotFound notfound = std::make_shared<OtHttpNotFoundClass>(s);
		notfound->setType(getMeta());
		return notfound;
	}

private:
	OtHttpResponse res;
};
