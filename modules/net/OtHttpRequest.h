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
//	OtHttpRequest
//

class OtHttpRequestClass;
typedef std::shared_ptr<OtHttpRequestClass> OtHttpRequest;


//
//	OtHttpRequestClass
//

class OtHttpRequestClass : public OtNetClass {
public:
	// clear all request fields
	void clear() {
		method.clear();
		path.clear();
		version.clear();
		headers.clear();
		params.clear();
		body.clear();
	}

	// set/get request parts
	void setMethod(const std::wstring& m) { method = m; }
	const std::wstring getMethod() { return method; }

	void setPath(const std::wstring& p) { path = p; }
	const std::wstring getPath() { return path; }

	void setVersion(const std::wstring& v) { version = v; }
	const std::wstring getVersion() { return version; }

	void setHeader(const std::wstring& name, const std::wstring& value) {
		headers[name] = value;
	}

	const bool hasHeader(const std::wstring& header) {
		return headers.find(header) != headers.end();
 	}

	const std::wstring getHeader(const std::wstring& header) {
		return headers[header];
	}

	void setParam(const std::wstring& name, const std::wstring& value) {
		params[name] = value;
	}

	const bool hasParam(const std::wstring& param) {
		return params.find(param) != params.end();
 	}

	const std::wstring getParam(const std::wstring& param) {
		return params[param];
	}

	void appendBody(const char* data, size_t length) {
		body.append(data, length);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtHttpRequestClass>(L"HttpRequest", OtNetClass::getMeta());

			type->set(L"method", OtFunctionCreate(&OtHttpRequestClass::getMethod));
			type->set(L"path", OtFunctionCreate(&OtHttpRequestClass::getPath));

			type->set(L"hasHeader", OtFunctionCreate(&OtHttpRequestClass::hasHeader));
			type->set(L"getHeader", OtFunctionCreate(&OtHttpRequestClass::getHeader));

			type->set(L"hasParam", OtFunctionCreate(&OtHttpRequestClass::hasParam));
			type->set(L"getParam", OtFunctionCreate(&OtHttpRequestClass::getParam));
		}

		return type;
	}

	// create a new object
	static OtHttpRequest create() {
		OtHttpRequest request = std::make_shared<OtHttpRequestClass>();
		request->setType(getMeta());
		return request;
	}

private:
	std::wstring method;
	std::wstring path;
	std::wstring version;
	std::map<std::wstring, std::wstring> headers;
	std::map<std::wstring, std::wstring> params;
	std::string body;
};
