//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>

#include "llhttp.h"

#include "OtHttp.h"
#include "OtHttpRequest.h"
#include "OtHttpResponse.h"
#include "OtHttpRouter.h"
#include "OtLibuv.h"


//
//	OtHttpSession
//

class OtHttpSessionClass;
using OtHttpSession = OtObjectPointer<OtHttpSessionClass>;

class OtHttpSessionClass : public OtHttpClass {
public:
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

private:
	// constructor
	friend class OtObjectPointer<OtHttpSessionClass>;
	OtHttpSessionClass(uv_stream_t* stream, OtHttpRouter router);

	// properties
	bool active = false;
	uint64_t lastRequest;

	OtHttpRequest request;
	OtHttpResponse response;

	OtHttpRouter router;

	uv_tcp_t uv_client;
	llhttp_settings_t settings;
	llhttp_t parser;
};
