//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "multipart/multipartparser.h"

#include "http.h"
#include "headers.h"


//
//	OtHttpRequest
//

class OtHttpRequestClass;
typedef std::shared_ptr<OtHttpRequestClass> OtHttpRequest;

class OtHttpRequestClass : public OtHttpClass {
public:
	// constructor
	OtHttpRequestClass();

	// clear all request fields
	void clear();

	// event handlers to deal with llhttp callbacks
	void onURL(const char *data, size_t length);
	void onHeaderField(const char *data, size_t length);
	void onHeaderValue(const char *data, size_t length);
	void onHeadersComplete(const std::string m, const std::string v);
	void onBody(const char *data, size_t length);
	void onMessageComplete();

	// event handlers to deal with multipart callbacks
	void onMultipartBegin();
	void onMultipartHeaderField(const char *data, size_t length);
	void onMultipartHeaderValue(const char *data, size_t length);
	void onMultipartHeadersComplete();
	void onMultipartData(const char *data, size_t length);
	void onMultipartEnd();

	// get request parts
	const std::string& getMethod() { return method; }
	const std::string& getURL() { return url; }
	const std::string& getPath() { return path; }
	const std::string& getVersion() { return version; }

	// access headers
	void setHeader(const std::string& name, const std::string& value);
	const bool hasHeader(const std::string& header);
	const bool headerIs(const std::string& header, const std::string& value);
	const std::string getHeader(const std::string& header);
	const OtObject getHeaders();

	// access query parameters
	void parseParams(const std::string& text);
	void setParam(const std::string& name, const std::string& value);
	const bool hasParam(const std::string& param);
	const std::string& getParam(const std::string& param);

	// access cookies
	void setCookie(const std::string& name, const std::string& value);
	const bool hasCookie(const std::string& cookie);
	const std::string& getCookie(const std::string& cookie);

	// access request body
	const std::string& getBody();

	// send request details back to browser
	const std::string debug();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtHttpRequest create();

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

	std::unordered_map<std::string, std::string> params;
	std::unordered_map<std::string, std::string> cookies;

	std::string body;

	std::string multipartBoundary;
	OtHttpHeaders multipartHeaders;
	std::string multipartFieldName;
	std::string multipartFileName;
	std::string multipartValue;

	std::string multipartFile;
	int multipartFD;

	multipartparser_callbacks multipartCallbacks;
	multipartparser multipartParser;
};
