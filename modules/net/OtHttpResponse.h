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
		setStatus(404);
		headers.clear();
		body.clear();
	}

	// set response status
	OtObject setStatus(int s) {
		status = s;

		switch (status) {
			//####### 1xx - Informational #######
			case 100: explanation = "Continue"; break;
			case 101: explanation = "Switching Protocols"; break;
			case 102: explanation = "Processing"; break;
			case 103: explanation = "Early Hints"; break;

			//####### 2xx - Successful #######
			case 200: explanation = "OK"; break;
			case 201: explanation = "Created"; break;
			case 202: explanation = "Accepted"; break;
			case 203: explanation = "Non-Authoritative Information"; break;
			case 204: explanation = "No Content"; break;
			case 205: explanation = "Reset Content"; break;
			case 206: explanation = "Partial Content"; break;
			case 207: explanation = "Multi-Status"; break;
			case 208: explanation = "Already Reported"; break;
			case 226: explanation = "IM Used"; break;

			//####### 3xx - Redirection #######
			case 300: explanation = "Multiple Choices"; break;
			case 301: explanation = "Moved Permanently"; break;
			case 302: explanation = "Found"; break;
			case 303: explanation = "See Other"; break;
			case 304: explanation = "Not Modified"; break;
			case 305: explanation = "Use Proxy"; break;
			case 307: explanation = "Temporary Redirect"; break;
			case 308: explanation = "Permanent Redirect"; break;

			//####### 4xx - Client Error #######
			case 400: explanation = "Bad Request"; break;
			case 401: explanation = "Unauthorized"; break;
			case 402: explanation = "Payment Required"; break;
			case 403: explanation = "Forbidden"; break;
			case 404: explanation = "Not Found"; break;
			case 405: explanation = "Method Not Allowed"; break;
			case 406: explanation = "Not Acceptable"; break;
			case 407: explanation = "Proxy Authentication Required"; break;
			case 408: explanation = "Request Timeout"; break;
			case 409: explanation = "Conflict"; break;
			case 410: explanation = "Gone"; break;
			case 411: explanation = "Length Required"; break;
			case 412: explanation = "Precondition Failed"; break;
			case 413: explanation = "Payload Too Large"; break;
			case 414: explanation = "URI Too Long"; break;
			case 415: explanation = "Unsupported Media Type"; break;
			case 416: explanation = "Range Not Satisfiable"; break;
			case 417: explanation = "Expectation Failed"; break;
			case 418: explanation = "I'm a teapot"; break;
			case 422: explanation = "Unprocessable Entity"; break;
			case 423: explanation = "Locked"; break;
			case 424: explanation = "Failed Dependency"; break;
			case 426: explanation = "Upgrade Required"; break;
			case 428: explanation = "Precondition Required"; break;
			case 429: explanation = "Too Many Requests"; break;
			case 431: explanation = "Request Header Fields Too Large"; break;
			case 451: explanation = "Unavailable For Legal Reasons"; break;

			//####### 5xx - Server Error #######
			case 500: explanation = "Internal Server Error"; break;
			case 501: explanation = "Not Implemented"; break;
			case 502: explanation = "Bad Gateway"; break;
			case 503: explanation = "Service Unavailable"; break;
			case 504: explanation = "Gateway Time-out"; break;
			case 505: explanation = "HTTP Version Not Supported"; break;
			case 506: explanation = "Variant Also Negotiates"; break;
			case 507: explanation = "Insufficient Storage"; break;
			case 508: explanation = "Loop Detected"; break;
			case 510: explanation = "Not Extended"; break; break;
			case 511: explanation = "Network Authentication Required"; break;

			default: explanation = ""; break;
		}

		return getSharedPtr();
	}

	// add response header
	OtObject setHeader(const std::string& name, const std::string& value) {
		headers[name] = value;
		return getSharedPtr();
	}

	// see if header is set
	const bool hasHeader(const std::string& header) {
		return headers.has(header);
 	}

	// set result body
	OtObject text(const std::string& text) {
		body = text;
		headers["Content-Type"] = "text/plain";
		setStatus(200);
		return getSharedPtr();
	}

	OtObject json(OtObject object) {
		body = object->repr();
		headers["Content-Type"] = "application/json";
		setStatus(200);
		return getSharedPtr();
	}

	// get size of body
	size_t getBodySize() {
		return body.size();
	}

	// put response in stream
	void toStream(std::stringstream& stream) {
		// put status and headers in stream
		stream << "HTTP/1.1 " << status << " " << explanation << "\r\n";

		for (auto &header : headers) {
			stream << header.first << ": " << header.second << "\r\n";
		}

		stream << "\r\n";
		stream << body;
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtHttpResponseClass>("HttpResponse", OtNetClass::getMeta());
			type->set("setStatus", OtFunctionCreate(&OtHttpResponseClass::setStatus));
			type->set("setHeader", OtFunctionCreate(&OtHttpResponseClass::setHeader));
			type->set("hasHeader", OtFunctionCreate(&OtHttpResponseClass::hasHeader));
			type->set("text", OtFunctionCreate(&OtHttpResponseClass::text));
			type->set("json", OtFunctionCreate(&OtHttpResponseClass::json));
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
	std::string explanation;
	OtHttpHeaders headers;
	std::string body;
};
