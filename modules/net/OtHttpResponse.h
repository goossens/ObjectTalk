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
//	OtHttpResponse
//

class OtHttpResponseClass;
typedef std::shared_ptr<OtHttpResponseClass> OtHttpResponse;


//
//	OtHttpResponseClass
//

class OtHttpResponseClass : public OtNetClass {
public:
	// clear all response fields
	void clear() {
		status = 200;
		explanation = L"OK";
		headers.clear();
	}

	// set response status
	void setStatus(int s) {
		status = s;

		switch (status) {
			//####### 1xx - Informational #######
			case 100: explanation = L"Continue"; break;
			case 101: explanation = L"Switching Protocols"; break;
			case 102: explanation = L"Processing"; break;
			case 103: explanation = L"Early Hints"; break;

			//####### 2xx - Successful #######
			case 200: explanation = L"OK"; break;
			case 201: explanation = L"Created"; break;
			case 202: explanation = L"Accepted"; break;
			case 203: explanation = L"Non-Authoritative Information"; break;
			case 204: explanation = L"No Content"; break;
			case 205: explanation = L"Reset Content"; break;
			case 206: explanation = L"Partial Content"; break;
			case 207: explanation = L"Multi-Status"; break;
			case 208: explanation = L"Already Reported"; break;
			case 226: explanation = L"IM Used"; break;

			//####### 3xx - Redirection #######
			case 300: explanation = L"Multiple Choices"; break;
			case 301: explanation = L"Moved Permanently"; break;
			case 302: explanation = L"Found"; break;
			case 303: explanation = L"See Other"; break;
			case 304: explanation = L"Not Modified"; break;
			case 305: explanation = L"Use Proxy"; break;
			case 307: explanation = L"Temporary Redirect"; break;
			case 308: explanation = L"Permanent Redirect"; break;

			//####### 4xx - Client Error #######
			case 400: explanation = L"Bad Request"; break;
			case 401: explanation = L"Unauthorized"; break;
			case 402: explanation = L"Payment Required"; break;
			case 403: explanation = L"Forbidden"; break;
			case 404: explanation = L"Not Found"; break;
			case 405: explanation = L"Method Not Allowed"; break;
			case 406: explanation = L"Not Acceptable"; break;
			case 407: explanation = L"Proxy Authentication Required"; break;
			case 408: explanation = L"Request Timeout"; break;
			case 409: explanation = L"Conflict"; break;
			case 410: explanation = L"Gone"; break;
			case 411: explanation = L"Length Required"; break;
			case 412: explanation = L"Precondition Failed"; break;
			case 413: explanation = L"Payload Too Large"; break;
			case 414: explanation = L"URI Too Long"; break;
			case 415: explanation = L"Unsupported Media Type"; break;
			case 416: explanation = L"Range Not Satisfiable"; break;
			case 417: explanation = L"Expectation Failed"; break;
			case 418: explanation = L"I'm a teapot"; break;
			case 422: explanation = L"Unprocessable Entity"; break;
			case 423: explanation = L"Locked"; break;
			case 424: explanation = L"Failed Dependency"; break;
			case 426: explanation = L"Upgrade Required"; break;
			case 428: explanation = L"Precondition Required"; break;
			case 429: explanation = L"Too Many Requests"; break;
			case 431: explanation = L"Request Header Fields Too Large"; break;
			case 451: explanation = L"Unavailable For Legal Reasons"; break;

			//####### 5xx - Server Error #######
			case 500: explanation = L"Internal Server Error"; break;
			case 501: explanation = L"Not Implemented"; break;
			case 502: explanation = L"Bad Gateway"; break;
			case 503: explanation = L"Service Unavailable"; break;
			case 504: explanation = L"Gateway Time-out"; break;
			case 505: explanation = L"HTTP Version Not Supported"; break;
			case 506: explanation = L"Variant Also Negotiates"; break;
			case 507: explanation = L"Insufficient Storage"; break;
			case 508: explanation = L"Loop Detected"; break;
			case 510: explanation = L"Not Extended"; break; break;
			case 511: explanation = L"Network Authentication Required"; break;

			default: explanation = L""; break;
		}
	}

	// add response header
	void setHeader(const std::wstring& name, const std::wstring& value) {
		headers[name] = value;
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtHttpResponseClass>(L"HttpResponse", OtNetClass::getMeta());
		}

		return type;
	}

	// create a new object
	static OtHttpResponse create() {
		OtHttpResponse response = std::make_shared<OtHttpResponseClass>();
		response->setType(getMeta());
		return response;
	}

private:
	int status;
	std::wstring explanation;
	std::map<std::wstring, std::wstring> headers;
};
