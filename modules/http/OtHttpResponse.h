//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtHttp.h"
#include "OtHttpHeaders.h"
#include "OtLibuv.h"


//
//	OtHttpResponse
//

class OtHttpResponseClass;
using OtHttpResponse = OtObjectPointer<OtHttpResponseClass>;

class OtHttpResponseClass : public OtHttpClass {
public:
	// specify the output stream
	void setStream(uv_stream_t* s);

	// clear all response fields
	void clear();

	// set response status
	OtObject setStatus(int s);

	// access headers
	OtObject setHeader(const std::string& name, const std::string& value);
	const bool hasHeader(const std::string& header);
	void sendHeaders();

	// write string as part of body
	OtObject write(const char* data, size_t size);
	OtObject write(const std::string& data);

	// end the response
	OtObject end();

	// send body
	OtObject send(const std::string& text);

	// send result as jSON
	OtObject json(OtObject object);

	// send a file as the response
	OtObject sendfile(const std::string& name);

	// handle file read events
	void onFileRead(ssize_t size);

	// download user file
	OtObject download(const std::string& name);

	// get type definition
	static OtType getMeta();

private:
	enum OtResponseState {
		START = 1,
		HEADERS_SENT,
		COMPLETE
	};

	OtResponseState responseState;
	int responseStatus;
	std::string explanation;
	OtHttpHeaders headers;

	uv_stream_t* clientStream;
	uv_file uv_read_fd;
	uv_fs_t uv_read_req;
	char* uv_read_buffer;
};
