//	ObjectTalk Scripting Language
//	Copyright 1993-2020 Johan A. Goossens
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

#include "OtSystem.h"
#include "OtPath.h"
#include "OtOS.h"
#include "OtFS.h"


//
//	System module
//

extern "C" void init(OtObject context)
{
	context->set(L"System", OtClassClass::create(OtSystemClass::getMeta()));
	context->set(L"Path", OtClassClass::create(OtPathClass::getMeta()));
	context->set(L"OS", OtClassClass::create(OtOSClass::getMeta()));
	context->set(L"FS", OtClassClass::create(OtFSClass::getMeta()));
}
