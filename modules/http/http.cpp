//	ObjectTalk Scripting Language
//	Copyright 1993-2021 Johan A. Goossens
//
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.


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
#include "OtHttpHeaders.h"
#include "OtHttpRequest.h"
#include "OtHttpResponse.h"
#include "OtHttpRouter.h"
#include "OtHttpServer.h"


//
//	Network module
//

extern "C" void init(OtObject context) {
	context->set("Net", OtClassClass::create(OtHttpClass::getMeta()));
	context->set("URL", OtClassClass::create(OtURLClass::getMeta()));
	context->set("HttpRouter", OtClassClass::create(OtHttpRouterClass::getMeta()));
	context->set("HttpServer", OtClassClass::create(OtHttpServerClass::getMeta()));
}
