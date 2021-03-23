//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/exception.h"
#include "ot/libuv.h"
#include "ot/function.h"

#include "server.h"
#include "session.h"


//
// OtHttpTimer
//

class OtHttpTimerClass;
typedef std::shared_ptr<OtHttpTimerClass> OtHttpTimer;

class OtHttpTimerClass : public OtInternalClass {
public:
	OtHttpTimerClass() = default;
	OtHttpTimerClass(long wait, long repeat, OtContext ctx, OtObject cb) : context(ctx), callback(cb) {
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
	static OtHttpTimer create(long wait, long repeat, OtContext context, OtObject callback) {
		OtHttpTimer timer = std::make_shared<OtHttpTimerClass>(wait, repeat, context, callback);
		timer->setType(getMeta());
		return timer;
	}

private:
	uv_timer_t uv_timer;
	OtContext context;
	OtObject callback;
};


//
//	OtHttpServerClass::onConnect
//

void OtHttpServerClass::onConnect() {
	// create new session
	auto session = OtHttpSessionClass::create((uv_stream_t*) &uv_server, cast<OtHttpRouterClass>());
	sessions.push_back(session);
}


//
//	OtHttpServerClass::listen
//

OtObject OtHttpServerClass::listen(const std::string& ip, long port) {
	uv_tcp_init(uv_default_loop(), &uv_server);
	uv_server.data = (void*) this;

	int status;
	struct sockaddr_in address;

	status = uv_ip4_addr(ip.c_str(), port, &address);
	UV_CHECK_ERROR("uv_ip4_addr", status);

	status = uv_tcp_bind(&uv_server, (const struct sockaddr*) &address, 0);
	UV_CHECK_ERROR("uv_tcp_bind", status);

	status = uv_listen((uv_stream_t*) &uv_server, 128, [](uv_stream_t* socket, int status) {
		UV_CHECK_ERROR("uv_listen", status);
		((OtHttpServerClass*)(socket->data))->onConnect();
	});

	UV_CHECK_ERROR("uv_listen", status);
	return getSharedPtr();
}


//
//	OtHttpServerClass::run
//

void OtHttpServerClass::run() {
	// setup our session watchdog
	uv_timer_init(uv_default_loop(), &uv_watchdog);
	uv_watchdog.data = this;

	uv_timer_start(&uv_watchdog, [](uv_timer_t* handle) {
		auto server = (OtHttpServerClass*) (handle->data);

		// remove dead sessions
		server->sessions.erase(std::remove_if(
			server->sessions.begin(),
			server->sessions.end(),
			[] (const OtHttpSession& session) {
				return !session->isAlive();
			}), server->sessions.end());
	}, 0, 60 * 1000);

	// run the libuv loop
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);

	// stop the watchdog
	uv_timer_stop(&uv_watchdog);

	// properly close all libuv handles
	uv_walk(uv_default_loop(), [](uv_handle_t* handle, void* arg) {
		if (!uv_is_closing(handle))
			uv_close(handle, nullptr);
	}, nullptr);

	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	uv_loop_close(uv_default_loop());

	// drop all remaining sessions
	sessions.clear();
}


//
//	OtHttpServerClass::timer
//

OtObject OtHttpServerClass::timer(OtContext context, size_t count, OtObject* parameters) {
	if (count != 3) {
		OtExcept("HttpServer.timer expected 3 parameter not [%d]", count);
	}

	return OtHttpTimerClass::create(*parameters[0], *parameters[1], context, parameters[2]);
}


//
//	OtHttpServerClass::stop
//

void OtHttpServerClass::stop() {
	// use timer so "stop" transaction can complete
	uv_timer_init(uv_default_loop(), &uv_shutdown);

	uv_timer_start(&uv_shutdown, [](uv_timer_t* handle) {
		// close all handles which will end libuv's loop
		uv_walk(uv_default_loop(), [](uv_handle_t* handle, void* arg) {
			if (!uv_is_closing(handle)) {
				uv_close(handle, nullptr);
			}
		}, nullptr);
	}, 1000, 0);
}


//
//	OtHttpServerClass::getMeta
//

OtType OtHttpServerClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtHttpServerClass>("HttpServer", OtHttpRouterClass::getMeta());
		type->set("listen", OtFunctionClass::create(&OtHttpServerClass::listen));
		type->set("run", OtFunctionClass::create(&OtHttpServerClass::run));
		type->set("timer", OtFunctionClass::create(&OtHttpServerClass::timer));
		type->set("stop", OtFunctionClass::create(&OtHttpServerClass::stop));
	}

	return type;
}


//
//	OtHttpServerClass::create
//

OtHttpServer OtHttpServerClass::create() {
	OtHttpServer server = std::make_shared<OtHttpServerClass>();
	server->setType(getMeta());
	return server;
}
