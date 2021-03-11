//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <uv.h>
#include "llhttp/llhttp.h"
#include "multipart/multipartparser.h"

#define UV_CHECK_ERROR(action, status) if (status < 0) OT_EXCEPT("libuv error in %s: %s", action, uv_strerror(status))

#include "Ot.h"

#include "OtHttp.h"
#include "OtMimeType.h"
#include "OtURL.h"
#include "OtHttpTimer.h"
#include "OtHttpHeaders.h"
#include "OtHttpRequest.h"
#include "OtHttpResponse.h"
#include "OtHttpNotFound.h"
#include "OtHttpRouter.h"
#include "OtHttpServer.h"


//
//	Network module
//

extern "C" void init(OtContext context) {
	context->set("Net", OtClassClass::create(OtHttpClass::getMeta()));
	context->set("URL", OtClassClass::create(OtURLClass::getMeta()));
	context->set("HttpRouter", OtClassClass::create(OtHttpRouterClass::getMeta()));
	context->set("HttpServer", OtClassClass::create(OtHttpServerClass::getMeta()));
}
