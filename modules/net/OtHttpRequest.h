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
	void setMethod(const std::string& m) { method = m; }
	const std::string getMethod() { return method; }

	void setPath(const std::string& p) { path = p; }
	const std::string getPath() { return path; }

	void setVersion(const std::string& v) { version = v; }
	const std::string getVersion() { return version; }

	void setHeader(const std::string& name, const std::string& value) {
		headers[name] = value;
	}

	const bool hasHeader(const std::string& header) {
		return headers.find(header) != headers.end();
 	}

	const std::string getHeader(const std::string& header) {
		if (hasHeader(header)) {
			return headers[header];

		} else {
			return"";
		}
	}

	const bool headerIs(const std::string& header, const std::string& value) {
		return getHeader(header) == value;
 	}

	void setParam(const std::string& name, const std::string& value) {
		params[name] = value;
	}

	const bool hasParam(const std::string& param) {
		return params.find(param) != params.end();
 	}

	const std::string getParam(const std::string& param) {
		return params[param];
	}

	void appendBody(const char* data, size_t length) {
		body.append(data, length);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtHttpRequestClass>("HttpRequest", OtNetClass::getMeta());

			type->set("method", OtFunctionCreate(&OtHttpRequestClass::getMethod));
			type->set("path", OtFunctionCreate(&OtHttpRequestClass::getPath));

			type->set("hasHeader", OtFunctionCreate(&OtHttpRequestClass::hasHeader));
			type->set("getHeader", OtFunctionCreate(&OtHttpRequestClass::getHeader));

			type->set("hasParam", OtFunctionCreate(&OtHttpRequestClass::hasParam));
			type->set("getParam", OtFunctionCreate(&OtHttpRequestClass::getParam));
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
	std::string method;
	std::string path;
	std::string version;
	std::map<std::string, std::string> headers;
	std::map<std::string, std::string> params;
	std::string body;
};
