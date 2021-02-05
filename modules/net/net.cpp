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

#include "Ot.h"

#include "OtNet.h"
#include "OtURL.h"
#include "OtLoop.h"
#include "OtHttpHeaders.h"
#include "OtHttpRequest.h"
#include "OtHttpResponse.h"
#include "OtHttpRouter.h"
#include "OtHTTP.h"

//
//	Network module
//

extern "C" void init(OtObject context) {
	context->set(L"Net", OtClassClass::create(OtNetClass::getMeta()));
	context->set(L"URL", OtClassClass::create(OtURLClass::getMeta()));
	context->set(L"Loop", OtClassClass::create(OtLoopClass::getMeta()));
	context->set(L"HTTPRequest", OtClassClass::create(OtHttpRequestClass::getMeta()));
	context->set(L"HTTPResponse", OtClassClass::create(OtHttpResponseClass::getMeta()));
	context->set(L"HTTPRouter", OtClassClass::create(OtHttpRouterClass::getMeta()));
	context->set(L"HTTP", OtClassClass::create(OtHTTPClass::getMeta()));
}
