//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <llhttp/llhttp.h>

#include "ot/libuv.h"

#include "http.h"
#include "request.h"
#include "response.h"
#include "router.h"


//
//	OtHttpSession
//

class OtHttpSessionClass;
typedef std::shared_ptr<OtHttpSessionClass> OtHttpSession;

class OtHttpSessionClass : public OtHttpClass {
public:
	// constructor
	OtHttpSessionClass(uv_stream_t* stream, OtHttpRouter router);

	// destructor
	~OtHttpSessionClass();

	// close session
	void close();

	// see if session is still alive
	bool isAlive();

	// event handlers
	void onBegin();
	void onMessageComplete();
	void onRead(const uv_buf_t* buffer, ssize_t nread);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtHttpSession create(uv_stream_t* stream, OtHttpRouter router);

private:
	bool active = false;
	uint64_t lastRequest;

	OtHttpRequest request;
	OtHttpResponse response;

	OtHttpRouter router;

	uv_tcp_t uv_client;
	llhttp_settings_t settings;
	llhttp_t parser;
};
