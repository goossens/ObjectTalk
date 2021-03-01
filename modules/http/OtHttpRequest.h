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

class OtHttpRequestClass : public OtInternalClass {
public:
	// constructor
	OtHttpRequestClass() {
		// initialize multipart parser callbacks
		multipartparser_callbacks_init(&multipartCallbacks);

		multipartCallbacks.on_part_begin = [](multipartparser* parser) -> int {
			((OtHttpRequestClass*)(parser->data))->onMultipartBegin();
			return 0;
		};

		multipartCallbacks.on_header_field = [](multipartparser* parser, const char *at, size_t length) -> int {
			((OtHttpRequestClass*)(parser->data))->onMultipartHeaderField(at, length);
			return 0;
		};

		multipartCallbacks.on_header_value = [](multipartparser* parser, const char *at, size_t length) -> int {
			((OtHttpRequestClass*)(parser->data))->onMultipartHeaderValue(at, length);
			return 0;
		};

		multipartCallbacks.on_headers_complete = [](multipartparser* parser) -> int {
			((OtHttpRequestClass*)(parser->data))->onMultipartHeadersComplete();
			return 0;
		};

		multipartCallbacks.on_data = [](multipartparser* parser, const char *at, size_t length) -> int {
			((OtHttpRequestClass*)(parser->data))->onMultipartData(at, length);
			return 0;
		};

		multipartCallbacks.on_part_end = [](multipartparser* parser) -> int {
			((OtHttpRequestClass*)(parser->data))->onMultipartEnd();
			return 0;
		};
	}

	// clear all request fields
	void clear() {
		method.clear();
		url.clear();
		path.clear();
		version.clear();

		headerState = WAITING_FOR_NAME;
		headerName.clear();
		headerValue.clear();
		headers.clear();

		params.clear();
		cookies.clear();

		body.clear();

		multipartBoundary.clear();
	}

	// event handlers to deal with llhttp callbacks
	void onURL(const char *data, size_t length) {
		url.append(data, length);
	}

	void onHeaderField(const char *data, size_t length) {
		// push header if complete
		if (headerState == WAITING_FOR_VALUE) {
			setHeader(headerName, headerValue);
			headerState = WAITING_FOR_NAME;
			headerName.clear();
			headerValue.clear();
		}

		// collect header name
		headerName.append(data, length);
	}

	void onHeaderValue(const char *data, size_t length) {
		// collect header value
		headerValue.append(data, length);
		headerState = WAITING_FOR_VALUE;
	}

	void onHeadersComplete(const std::string m, const std::string v) {
		// save method and version
		method = m;
		version = v;

		// save last header if required
		if (headerState == WAITING_FOR_VALUE) {
			setHeader(headerName, headerValue);
		}

		// parse URL and extract path and query parameter
		size_t query = url.find('?');

		if (query == std::string::npos) {
			path = OtTextDecodeURL(url);

		} else {
			path = OtTextDecodeURL(url.substr(0, query));
			parseParams(url.substr(query + 1, std::string::npos));
		}

		// handle multipart form data
		if (OtTextContains(getHeader("Content-Type"), "multipart/form-data")) {
			auto type = getHeader("Content-Type");
			auto pos = type.find("boundary=");

			// find boundary string
			if (pos != std::string::npos) {
				multipartBoundary = type.substr(pos + 9);

				if (multipartBoundary.length() >= 2 &&
					multipartBoundary.front() == '"' &&
					multipartBoundary.back() == '"') {
					multipartBoundary = multipartBoundary.substr(1, multipartBoundary.size() - 2);
				}

				// initialize multipart parser
				multipartparser_init(&multipartParser, multipartBoundary.c_str());
				multipartParser.data = this;
			}
		}
	}

	void onBody(const char *data, size_t length) {
		// handle multipart if required
		if (multipartBoundary.size()) {
			auto parsed = multipartparser_execute(&multipartParser, &multipartCallbacks, data, length);

			if (parsed != length) {
				OT_EXCEPT("Invalid multipart", false);
			}

		} else {
			body.append(data, length);
		}
	}

	void onMultipartBegin() {
		// reset multipart header fields
		multipartHeaders.clear();
		multipartFieldName.clear();
		multipartFileName.clear();
		multipartFile.clear();
		multipartValue.clear();

		headerState = WAITING_FOR_NAME;
		headerName.clear();
		headerValue.clear();
	}

	void onMultipartHeaderField(const char *data, size_t length) {
		// push multipart header if complete
		if (headerState == WAITING_FOR_VALUE) {
			multipartHeaders.emplace(headerName, headerValue);
			headerState = WAITING_FOR_NAME;
			headerName.clear();
			headerValue.clear();
		}

		// collect multipart header name
		headerName.append(data, length);
	}

	void onMultipartHeaderValue(const char *data, size_t length) {
		// collect multipart header value
		headerValue.append(data, length);
		headerState = WAITING_FOR_VALUE;
	}

	void onMultipartHeadersComplete() {
		// handle last multipart header (if required)
		if (headerState == WAITING_FOR_VALUE) {
			multipartHeaders.emplace(headerName, headerValue);
		}

		// parse "Content-Disposition" header
		if (multipartHeaders.has("Content-Disposition")) {
			auto d = multipartHeaders.get("Content-Disposition");

			OtTextSplitTrimIterator(d.data(), d.data() + d.size(), ';', [&](const char *b, const char *e) {
				auto part = std::string(b, e - b);

				if (part.find("=") != std::string::npos) {
					std::string key;
					std::string val;

					OtTextSplitTrimIterator(b, e, '=', [&](const char *b2, const char *e2) {
						if (key.empty()) {
							key.assign(b2, e2);

						} else {
							if (*b2 == '"' && b2 < e2) {
								b2++;
							}

							if (*(e2 - 1) == '"' && e2 > b2) {
								e2--;
							}

							val.assign(b2, e2);
						}
					});

					if (key == "name") {
						multipartFieldName = val;

					} else if (key == "filename") {
						// get client file name
						multipartFileName = val;

						// create temporary file
						multipartFile = std::tmpnam(nullptr);
						multipartStream.open(multipartFile, std::ios::out | std::ios::binary);
					}
				}
			});

		} else {
			OT_EXCEPT("Content-Disposition missing in multipart", false);
		}
	}

	void onMultipartData(const char *data, size_t length) {
		// handle file uploads
		if (multipartFileName.size()) {
			multipartStream.write(data, length);

		} else {
			// handle field values
			multipartValue.append(data, length);
		}
	}

	void onMultipartEnd() {
		// handle file uploads
		if (multipartFileName.size()) {
			multipartStream.close();
			setParam(multipartFieldName, multipartFileName + "|" + multipartFile);

		} else {
			// handle field values
			setParam(multipartFieldName, multipartValue);
		}
	}

	void onMessageComplete() {
		// handle form parameters
		if (headerIs("Content-Type", "application/x-www-form-urlencoded")) {
			parseParams(body);
		}
	}

	// get request parts
	const std::string& getMethod() { return method; }
	const std::string& getURL() { return url; }
	const std::string& getPath() { return path; }
	const std::string& getVersion() { return version; }

	// access headers
	void setHeader(const std::string& name, const std::string& value) {
		headers.emplace(name, value);

		if (OtTextCaseEqual(name, "cookie")) {
			OtTextSplitIterator(value.data(), value.data() + value.size(), ';', [&](const char *b, const char *e) {
				std::string key;
				std::string val;

				OtTextSplitIterator(b, e, '=', [&](const char *b2, const char *e2) {
					if (key.empty()) {
						key.assign(b2, e2);

					} else {
						val.assign(b2, e2);
					}
				});

				setCookie(OtTextDecodeURL(OtTextTrim(key)), OtTextDecodeURL(OtTextTrim(val)));
			});
		}
	}

	const bool hasHeader(const std::string& header) {
		return headers.find(header) != headers.end();
	}

	const bool headerIs(const std::string& header, const std::string& value) {
		return getHeader(header) == value;
	}

	const std::string getHeader(const std::string& header) {
		return headers.get(header);
	}

	const OtObject getHeaders() {
		OtDict dict = OtDictClass::create();

		for (auto i = headers.begin(); i != headers.end(); i++) {
			if (dict->find(i->first) == dict->end()) {
				dict->operator[] (i->first) = OtStringClass::create(i->second);

			} else {
				std::string value = *(dict->operator[] (i->first)) + "; " + i->second;
				dict->operator[] (i->first) = OtStringClass::create(value);
			}
		}

		return dict;
	}

	// access query parameters
	void parseParams(const std::string& text) {
		OtTextSplitIterator(text.data(), text.data() + text.size(), '&', [&](const char *b, const char *e) {
			std::string key;
			std::string value;

			OtTextSplitIterator(b, e, '=', [&](const char *b2, const char *e2) {
				if (key.empty()) {
					key.assign(b2, e2);

				} else {
					value.assign(b2, e2);
				}
			});

			setParam(OtTextDecodeURL(key), OtTextDecodeURL(value));
		});
	}

	void setParam(const std::string& name, const std::string& value) {
		params[name] = value;
	}

	const bool hasParam(const std::string& param) {
		return params.find(param) != params.end();
	}

	const std::string& getParam(const std::string& param) {
		return params[param];
	}

	// manipulate cookies
	void setCookie(const std::string& name, const std::string& value) {
		cookies[name] = value;
	}

	const bool hasCookie(const std::string& cookie) {
		return cookies.find(cookie) != params.end();
	}

	const std::string& getCookie(const std::string& cookie) {
		return cookies[cookie];
	}

	// access cookies
	const std::string& getBody() {
		return body;
	}

	const std::string debug() {
		std::stringstream stream;

		stream << "Method: " << method << std::endl;
		stream << "URL: " << url << std::endl;
		stream << "Path: " << path << std::endl;
		stream << "Version: " << version << std::endl;

		stream << std::endl << "Headers: " << std::endl << "--------" << std::endl;

		for (auto i = headers.begin(); i != headers.end(); i++) {
			stream << i->first << "=" << i->second << std::endl;
		}

		stream << std::endl << "Query Parameters: " << std::endl << "-----------------" << std::endl;

		for (auto i = params.begin(); i != params.end(); i++) {
			stream << i->first << "=" << i->second << std::endl;
		}

		stream << std::endl << "Cookies: " << std::endl << "--------" << std::endl;

		for (auto i = cookies.begin(); i != cookies.end(); i++) {
			stream << i->first << "=" << i->second << std::endl;
		}

		return stream.str();
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtHttpRequestClass>("HttpRequest", OtInternalClass::getMeta());

			type->set("getMethod", OtFunctionCreate(&OtHttpRequestClass::getMethod));
			type->set("getURL", OtFunctionCreate(&OtHttpRequestClass::getURL));
			type->set("getPath", OtFunctionCreate(&OtHttpRequestClass::getPath));
			type->set("getVersion", OtFunctionCreate(&OtHttpRequestClass::getVersion));

			type->set("hasHeader", OtFunctionCreate(&OtHttpRequestClass::hasHeader));
			type->set("headerIs", OtFunctionCreate(&OtHttpRequestClass::hasHeader));
			type->set("getHeader", OtFunctionCreate(&OtHttpRequestClass::getHeader));
			type->set("getHeaders", OtFunctionCreate(&OtHttpRequestClass::getHeaders));

			type->set("hasParam", OtFunctionCreate(&OtHttpRequestClass::hasParam));
			type->set("getParam", OtFunctionCreate(&OtHttpRequestClass::getParam));

			type->set("hasCookie", OtFunctionCreate(&OtHttpRequestClass::hasCookie));
			type->set("getCookie", OtFunctionCreate(&OtHttpRequestClass::getCookie));

			type->set("getBody", OtFunctionCreate(&OtHttpRequestClass::getBody));

			type->set("debug", OtFunctionCreate(&OtHttpRequestClass::debug));
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
	std::string url;
	std::string path;
	std::string version;

	enum OtHttpHeaderState {
		WAITING_FOR_NAME = 1,
		WAITING_FOR_VALUE
	};

	OtHttpHeaderState headerState;
	std::string headerName;
	std::string headerValue;
	OtHttpHeaders headers;

	std::map<std::string, std::string> params;
	std::map<std::string, std::string> cookies;

	std::string body;

	std::string multipartBoundary;
	OtHttpHeaders multipartHeaders;
	std::string multipartFieldName;
	std::string multipartFileName;
	std::string multipartValue;

	std::string multipartFile;
	std::ofstream multipartStream;

	multipartparser_callbacks multipartCallbacks;
	multipartparser multipartParser;
};
