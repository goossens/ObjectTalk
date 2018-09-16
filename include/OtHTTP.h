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

class OtHTTPClass : public OtNetClass
{
public:
	// constructors
	OtHTTPClass() {}

	// class to represent an HTTP request
	class OtHTTPRequestClass : public OtInternalClass
	{
	public:
		// constructor
		OtHTTPRequestClass() {}
		OtHTTPRequestClass(const std::string& m, OtURI u, const std::string& v) { method = m; uri = u; version = v; }

		// get member data
		OtObject getMethod() { return OtObjectCreate(method); }
		OtObject getURI() { return uri; }
		OtObject getVersion() { return OtObjectCreate(version); }

		// get type definition
		static OtType getMeta()
		{
			static OtType type = nullptr;

			if (!type)
			{
				type = OtTypeClass::create<OtHTTPRequestClass>("HttpRequest", OtInternalClass::getMeta());
				type->set("method", OtFunctionCreate(&OtHTTPRequestClass::getMethod));
				type->set("uri", OtFunctionCreate(&OtHTTPRequestClass::getURI));
				type->set("version", OtFunctionCreate(&OtHTTPRequestClass::getVersion));
			}

			return type;
		}

		// create a new object
		static OtObject create(const std::string& m, OtURI u, const std::string& v)
		{
			return std::make_shared<OtHTTPRequestClass>(m, u, v)->setType(getMeta());
		}

	private:
		std::string method;
		OtURI uri;
		std::string version;
	};

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtHTTPClass>("HTTP", OtNetClass::getMeta());
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
