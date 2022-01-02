//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "ot/httprequest.h"
#include "ot/httpresponse.h"
#include "ot/httprouter.h"
#include "ot/internal.h"


//
//	OtHttpNext
//

class OtHttpNextClass;
typedef std::shared_ptr<OtHttpNextClass> OtHttpNext;

class OtHttpNextClass : public OtInternalClass {
public:
	OtHttpNextClass() = default;
	OtHttpNextClass(OtHttpRouter r, size_t i, OtHttpRequest q, OtHttpResponse s, OtObject n)
		: router(r), index(i), req(q), res(s), next(n) {}

	// execute next
	void call() {
		router.lock()->runHandler(index, req, res, next);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtHttpNextClass>("HttpNext", OtInternalClass::getMeta());
			type->set("__call__", OtFunctionClass::create(&OtHttpNextClass::call));
		}

		return type;
	}

	// create a new object
	static OtHttpNext create(OtHttpRouter r, size_t i, OtHttpRequest q, OtHttpResponse s, OtObject n) {
		OtHttpNext next = std::make_shared<OtHttpNextClass>(r, i, q, s, n);
		next->setType(getMeta());
		return next;
	}

private:
	std::weak_ptr<OtHttpRouterClass> router;
	size_t index;
	OtHttpRequest req;
	OtHttpResponse res;
	OtObject next;
};
