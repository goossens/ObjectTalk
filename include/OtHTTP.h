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


//
//	OtHTTP
//

class OtHTTPClass;
typedef std::shared_ptr<OtHTTPClass> OtHTTP;


//
//	OtHTTPClass
//

class OtHTTPClass : public OtOSClass
{
public:
	OtHTTPClass() {}

	// class to represent an HTTP request
	class OtHttpRequestClass : public OtInternalClass
	{
	public:
		// get type definition
		static OtType getMeta()
		{
			static OtType type = nullptr;

			if (!type)
			{
				type = OtTypeClass::create<OtHttpRequestClass>("HttpRequest", OtInternalClass::getMeta());
				type->set("__deref__", OtFunctionCreate(&OtHttpRequestClass::deref));
				type->set("__assign__", OtFunctionCreate(&OtHttpRequestClass::assign));
			}

			return type;
		}

		// create a new object
		static OtObject create(OtArray a, size_t i) { return std::make_shared<OtHttpRequestClass>(a, i)->setType(getMeta()); }
		
	private:
		std::string method;
		OtURIClass uri;
		std::string 
	};

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtHTTPClass>("HTTP", OtOSClass::getMeta());

			type->set("getcwd", OtFunctionCreate(&OtHTTPClass::getcwd));
			type->set("ls", OtFunctionCreate(&OtHTTPClass::ls));
			type->set("filesize", OtFunctionCreate(&OtHTTPClass::filesize));
			type->set("rm", OtFunctionCreate(&OtHTTPClass::rm));
		}

		return type;
	}

	// create a new object
	static OtHTTP create()
	{
		OtHTTP fs = std::make_shared<OtHTTPClass>();
		fs->setType(getMeta());
		return fs;
	}
};
