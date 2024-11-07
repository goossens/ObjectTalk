//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtFunction.h"
#include "OtLibuv.h"
#include "OtHttpServer.h"
#include "OtHttpSession.h"


//
//	OtHttpServerClass::OtHttpServerClass
//

OtHttpServerClass::OtHttpServerClass() {
	// setup our session watchdog
	uv_timer_init(uv_default_loop(), &uv_watchdog);
	uv_watchdog.data = this;

	uv_timer_start(&uv_watchdog, [](uv_timer_t* handle) {
		((OtHttpServerClass*) (handle->data))->cleanup();
	}, 0, 60 * 1000);
}


//
//	OtHttpServerClass::~OtHttpServerClass
//

OtHttpServerClass::~OtHttpServerClass() {
	// stop the watchdog
	uv_timer_stop(&uv_watchdog);
}


//
//	OtHttpServerClass::init
//

void OtHttpServerClass::init(OtObject object) {
	object->expectKindOf("HttpRouter");
	router = OtHttpRouter(object);
};


//
//	OtHttpServerClass::onConnect
//

void OtHttpServerClass::onConnect() {
	// create new session
	auto session = OtHttpSession::create((uv_stream_t*) &uv_server, router);
	sessions.push_back(session);
}


//
//	OtHttpServerClass::listen
//

OtObject OtHttpServerClass::listen(const std::string& ip, int port) {
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
	return OtHttpServer(this);
}


//
//	OtHttpServerClass::cleanup
//

void OtHttpServerClass::cleanup() {
	// remove dead sessions
	sessions.erase(std::remove_if(sessions.begin(), sessions.end(), [](OtHttpSession& session) {
		return !session->isAlive();
	}), sessions.end());
}


//
//	OtHttpServerClass::getMeta
//

OtType OtHttpServerClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtHttpServerClass>("HttpServer", OtHttpClass::getMeta());
		type->set("__init__", OtFunction::create(&OtHttpServerClass::init));
		type->set("listen", OtFunction::create(&OtHttpServerClass::listen));
	}

	return type;
}
