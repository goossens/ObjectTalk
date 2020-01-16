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

		// parse request header
		bool parse(std::istream &stream)
		{
			std::wstring line;
			std::getline(stream, line);
			std::size_t start, end;

			if ((end = line.find_first_of(' ')) == std::wstring::npos)
				return false;

			method = line.substr(0, end);
			start = line.find_first_not_of(' ', end);

			if ((end = line.find_first_of(' ', start)) == std::wstring::npos)
				return false;

			uri = OtURIClass::create(line.substr(start, end));
			start = line.find_first_not_of(' ', end);

			version = line.substr(start, line.find_last_not_of("\n\r") + 1);

			headers.clear();
			std::getline(stream, line);

			while ((end = line.find_first_of(':')) != std::wstring::npos)
			{
				headers.push_back(line.substr(0, line.find_last_not_of("\n\r") + 1));
				std::getline(stream, line);
			}

			return true;
		}

		// get member data
		OtObject getMethod() { return OtObjectCreate(method); }
		OtObject getURI() { return uri; }
		OtObject getVersion() { return OtObjectCreate(version); }

		std::size_t getHeaderCount() { return headers.size(); }
		std::wstring getHeader(long index) { return headers[index]; }

		OtObject getHeaderValues(const std::wstring& name)
		{
			OtArray values = OtArrayClass::create();

			for (auto& header : headers)
			{
				std::size_t sep = header.find_first_of(':');
				std::wstring key = header.substr(0, sep);
				std::wstring value = header.substr(header.find_first_not_of(" \t", sep + 1), std::wstring::npos);

				if (OtTextCaseInsensitiveEqual(key, name))
					values->append(OtStringClass::create(value));
			}

			return values;
		}

		// get type definition
		static OtType getMeta()
		{
			static OtType type = nullptr;

			if (!type)
			{
				type = OtTypeClass::create<OtHTTPRequestClass>(L"HttpRequest", OtInternalClass::getMeta());
				type->set(L"method", OtFunctionCreate(&OtHTTPRequestClass::getMethod));
				type->set(L"uri", OtFunctionCreate(&OtHTTPRequestClass::getURI));
				type->set(L"version", OtFunctionCreate(&OtHTTPRequestClass::getVersion));
				type->set(L"headers", OtFunctionCreate(&OtHTTPRequestClass::getHeaderCount));
				type->set(L"header", OtFunctionCreate(&OtHTTPRequestClass::getHeader));
				type->set(L"headervalues", OtFunctionCreate(&OtHTTPRequestClass::getHeaderValues));
			}

			return type;
		}

	private:
		std::wstring method;
		OtURI uri;
		std::wstring version;
		std::vector<std::wstring> headers;
	};

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtHTTPClass>(L"HTTP", OtHTTPClass::getMeta());
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
