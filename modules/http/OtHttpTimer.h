//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
// OtHttpTimer
//

class OtHttpTimerClass;
typedef std::shared_ptr<OtHttpTimerClass> OtHttpTimer;

class OtHttpTimerClass : public OtInternalClass {
public:
	OtHttpTimerClass() = default;
	OtHttpTimerClass(long wait, long repeat, OtObject ctx, OtObject cb) : context(ctx), callback(cb) {
		uv_timer_init(uv_default_loop(), &uv_timer);
		uv_timer.data = this;

		uv_timer_start(&uv_timer, [](uv_timer_t* handle) {
			OtHttpTimerClass* timer = (OtHttpTimerClass*) (handle->data);
			timer->callback->get("__call__")->operator ()(timer->context, 1, &timer->callback);
		}, wait, repeat);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtHttpTimerClass>("HttpTimer", OtInternalClass::getMeta());
		}

		return type;
	}

	// create a new object
	static OtHttpTimer create(long wait, long repeat, OtObject context, OtObject callback) {
		OtHttpTimer timer = std::make_shared<OtHttpTimerClass>(wait, repeat, context, callback);
		timer->setType(getMeta());
		return timer;
	}

private:
	uv_timer_t uv_timer;
	OtObject context;
	OtObject callback;
};
