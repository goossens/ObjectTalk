//	ObjectTalk Scripting Language
//	Copyright 1993-2019 Johan A. Goossens
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
//	OtObjectTalk
//

class OtObjectTalk
{
public:
	// create a default ObjectTalk context
	static OtObject createDefaultContext()
	{
		// create context
		OtObject context = OtObjectClass::create();

		// add default constants
		context->set(L"true", OtBooleanClass::create(true));
		context->set(L"false", OtBooleanClass::create(false));
		context->set(L"null", nullptr);

		// add default functions
		context->set(L"assert", OtFunctionClass::create([] (OtObject context, size_t c, OtObject* p)->OtObject
		{
			if (c != 1)
				OT_EXCEPT(L"Function [assert] expects 1 parameter, %d given", c);

			OtObjectTalk ot;
			std::wstring assertion = p[0]->operator std::wstring();
			OtObject result = ot.processText(assertion, context);

			if (!result->operator bool())
				OT_EXCEPT(L"Assertion [%s] failed", assertion.c_str());

			return nullptr;
		}));

		context->set(L"import", OtFunctionClass::create([] (OtObject context, size_t c, OtObject* p)->OtObject
		{
			if (c != 1)
				OT_EXCEPT(L"Function [import] expects 1 parameter, %d given", c);

			return processFile(p[0]->operator std::wstring(), context);
		}));

		context->set(L"run", OtFunctionClass::create([] (OtObject, size_t c, OtObject* p)->OtObject
		{
			if (c != 1)
				OT_EXCEPT(L"Function [run] expects 1 parameter, %d given", c);

			OtObject context = createDefaultContext();
			std::wstring path = p[0]->operator std::wstring();
			context->set(L"__FILE__", OtStringClass::create(path));

			return processFile(path, context);
		}));

		context->set(L"print", OtFunctionClass::create([] (OtObject, size_t c, OtObject* p)->OtObject
		{
			for (size_t i = 0; i < c; i++)
				std::wcout << (std::wstring) *p[i];

			std::wcout << std::endl;
			return nullptr;
		}));

		// add default classes
		context->set(L"Object", OtClassClass::create(OtObjectClass::getMeta()));

		context->set(L"Internal", OtClassClass::create(OtInternalClass::getMeta()));
		context->set(L"Class", OtClassClass::create(OtClassClass::getMeta()));

		context->set(L"Primitive", OtClassClass::create(OtPrimitiveClass::getMeta()));
		context->set(L"Boolean", OtClassClass::create(OtBooleanClass::getMeta()));
		context->set(L"Integer", OtClassClass::create(OtIntegerClass::getMeta()));
		context->set(L"Real", OtClassClass::create(OtRealClass::getMeta()));
		context->set(L"String", OtClassClass::create(OtStringClass::getMeta()));
		context->set(L"Function", OtClassClass::create(OtFunctionClass::getMeta()));

		context->set(L"Collection", OtClassClass::create(OtCollectionClass::getMeta()));
		context->set(L"Array", OtClassClass::create(OtArrayClass::getMeta()));
		context->set(L"Dict", OtClassClass::create(OtDictClass::getMeta()));

		// return default context
		return context;
	}

	// compile and run ObjectTalk text
	static OtObject processText(const std::wstring& text, OtObject context)
	{
		// compile code, run it and return result
		OtCompiler compiler;
		OtCode code = compiler.compile(text);
		return code->operator ()(context);
	}

	// compile and run an ObjectTalk file
	static OtObject processFile(const std::wstring& path, OtObject context)
	{
		// get text from file and process it
		std::ifstream stream(path);
		std::wstringstream buffer;
		buffer << stream.rdbuf();
		return processText(buffer.str(), context);
	}

private:
	// constructor
	OtObjectTalk() {}
};
