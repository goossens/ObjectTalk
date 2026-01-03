//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
using OtHttpTimer = OtObjectPointer<OtHttpTimerClass>;

class OtHttpTimerClass : public OtInternalClass {
public:
	// get type definition
	static OtType getMeta() {
		static OtType type;

		if (!type) {
			type = OtType::create<OtHttpTimerClass>("HttpTimer", OtInternalClass::getMeta());
		}

		return type;
	}

protected:
	// constructor
	friend class OtObjectPointer<OtHttpTimerClass>;
	OtHttpTimerClass() = default;
	OtHttpTimerClass(int64_t interval, OtObject cb) : callback(cb) {
		// sanity check
		OtCallbackValidate(callback, 0);

		uv_timer_init(uv_default_loop(), &uv_timer);
		uv_timer.data = this;

		uv_timer_start(&uv_timer, [](uv_timer_t* handle) {
			OtHttpTimerClass* timer = (OtHttpTimerClass*) (handle->data);
			OtVM::callMemberFunction(timer->callback, "__call__");
		}, interval, 1);
	}

private:
	// properties
	uv_timer_t uv_timer;
	OtObject callback;
};
