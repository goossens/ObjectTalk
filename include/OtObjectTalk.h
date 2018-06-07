//	ObjectTalk Scripting Language
//	Copyright 1993-2018 Johan A. Goossens
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


#pragma once


//
//  Include files
//

#include <cctype>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
	 
#include "OtFormat.h"
#include "OtException.h"

#include "OtDefs.h"
#include "OtType.h"
#include "OtObject.h"
#include "OtFunction.h"
#include "OtBoundFunction.h"
#include "OtClass.h"

#include "OtTypeImpl.h"
#include "OtObjectImpl.h"

#include "OtBoolean.h"
#include "OtInteger.h"
#include "OtReal.h"
#include "OtString.h"

#include "OtArray.h"
#include "OtDict.h"

#include "OtCode.h"
#include "OtCodeFunction.h"

#include "OtScanner.h"
#include "OtCompiler.h"


//
//  OtObjectTalk
//

class OtObjectTalk
{
public:
	// constructor
	OtObjectTalk() {}

	// create a default object talk context
	OtObject createDefaultContext()
	{
		// create global context
		OtObject context = OtObjectClass::create();
		context->set("global", context);

		// add default constants
		context->set("true", OtBooleanClass::create(true));
		context->set("false", OtBooleanClass::create(false));
		context->set("null", nullptr);

		// add default functions
		context->set("assert", OtFunctionClass::create([] (OtObject context, size_t c, OtObject* p)->OtObject
		{
			if (c != 1)
				OT_EXCEPT("Function expects 1 parameter, %d given", c);

			OtObjectTalk ot;
			std::string assertion = p[0]->operator std::string();
			OtObject result = ot.processText(assertion, context);
			
			if (!result->operator bool())
				OT_EXCEPT("Assertion [%s] failed", assertion.c_str());

			return nullptr;
		}));

		context->set("print", OtFunctionClass::create([] (OtObject, size_t c, OtObject* p)->OtObject
		{
			for (size_t i = 0; i < c; i++)
				std::cout << (std::string) *p[i];

			std::cout << std::endl;
			return nullptr;
		}));

		// add default classes
		context->set("Object", OtClassClass::create(OtObjectClass::getMeta()));
		context->set("Class", OtClassClass::create(OtClassClass::getMeta()));

		context->set("Boolean", OtClassClass::create(OtBooleanClass::getMeta()));
		context->set("Integer", OtClassClass::create(OtIntegerClass::getMeta()));
		context->set("Real", OtClassClass::create(OtRealClass::getMeta()));
		context->set("String", OtClassClass::create(OtStringClass::getMeta()));
		context->set("Function", OtClassClass::create(OtFunctionClass::getMeta()));

		context->set("Array", OtClassClass::create(OtArrayClass::getMeta()));
		context->set("Dict", OtClassClass::create(OtDictClass::getMeta()));

		// return default context
		return context;
	}

	// compile and run ObjectTalk text
	OtObject processText(const std::string& text, OtObject context=nullptr)
	{
		// compile the code
		OtCompiler compiler;
		OtCode code = compiler.compile(text);

		// create default context if required
		if (!context)
			context = createDefaultContext();

		// execute code and return result
		return code->execute(context);
	}

	// compile and run an ObjectTalk file
	OtObject processFile(const std::string& filename, OtObject context=nullptr)
	{
		// get text from file and process
		std::ifstream stream(filename);
		std::stringstream buffer;
		buffer << stream.rdbuf();
		return processText(buffer.str(), context);
	}
};
