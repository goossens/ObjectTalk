//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtHttpRequest.h"
#include "OtHttpResponse.h"
#include "OtHttpRouter.h"
#include "OtInternal.h"


//
//	OtHttpNext
//

class OtHttpNextClass;
using OtHttpNext = OtObjectPointer<OtHttpNextClass>;

class OtHttpNextClass : public OtInternalClass {
public:
	OtHttpNextClass() = default;
	OtHttpNextClass(OtHttpRouterClass* r, size_t i, OtHttpRequest q, OtHttpResponse s, OtObject n)
		: router(r), index(i), req(q), res(s), next(n) {}

	// execute next
	void call() {
		router->runHandler(index, req, res, next);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type;

		if (!type) {
			type = OtType::create<OtHttpNextClass>("HttpNext", OtInternalClass::getMeta());
			type->set("__call__", OtFunction::create(&OtHttpNextClass::call));
		}

		return type;
	}

private:
	OtHttpRouterClass* router;
	size_t index;
	OtHttpRequest req;
	OtHttpResponse res;
	OtObject next;
};
