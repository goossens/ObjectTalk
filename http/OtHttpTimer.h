//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtCallback.h"
#include "OtInternal.h"
#include "OtLibuv.h"
#include "OtVM.h"


//
// OtHttpTimer
//

class OtHttpTimerClass;
typedef std::shared_ptr<OtHttpTimerClass> OtHttpTimer;

class OtHttpTimerClass : public OtInternalClass {
public:
	OtHttpTimerClass() = default;
	OtHttpTimerClass(int64_t wait, int64_t repeat, OtObject cb) : callback(cb) {
		// sanity check
		OtCallbackValidate(callback, 0);

		uv_timer_init(uv_default_loop(), &uv_timer);
		uv_timer.data = this;

		uv_timer_start(&uv_timer, [](uv_timer_t* handle) {
			OtHttpTimerClass* timer = (OtHttpTimerClass*) (handle->data);
			OtVM::instance()->callMemberFunction(timer->callback, "__call__");
		}, wait, repeat);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type;

		if (!type) {
			type = OtTypeClass::create<OtHttpTimerClass>("HttpTimer", OtInternalClass::getMeta());
		}

		return type;
	}

	// create a new object
	static OtHttpTimer create(int64_t wait, int64_t repeat, OtObject callback) {
		OtHttpTimer timer = std::make_shared<OtHttpTimerClass>(wait, repeat, callback);
		timer->setType(getMeta());
		return timer;
	}

private:
	uv_timer_t uv_timer;
	OtObject callback;
};
