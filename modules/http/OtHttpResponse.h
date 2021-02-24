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

class OtHttpResponseClass : public OtInternalClass {
public:
	// specify the output stream
	void setStream(uv_stream_t* s) {
		clientStream = s;
	}

	// clear all response fields
	void clear() {
		state = START;
		setStatus(404);
		headers.clear();
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
		if (state != START) {
			OT_EXCEPT("HttpResponse can't send headers after body transmission has started", false);
		}

		headers.emplace(name, value);
		return getSharedPtr();
	}

	// see if header is set
	const bool hasHeader(const std::string& header) {
		return headers.has(header);
	}

	// send headers to stream
	void sendHeaders() {
		std::stringstream stream;

		// put status and headers in stream
		stream << "HTTP/1.1 " << status << " " << explanation << "\r\n";

		for (auto &header : headers) {
			stream << header.first << ": " << header.second << "\r\n";
		}

		stream << "\r\n";

		// send response
		std::string text = stream.str();
		uv_buf_t buffer = uv_buf_init(strndup((char*) text.c_str(), text.size()), text.size());
		uv_write_t* uv_write_req = new uv_write_t;
		uv_write_req->data = buffer.base;

		uv_write(uv_write_req, clientStream, &buffer, 1, [](uv_write_t* req, int status) {
			delete (char*) (req->data);
			delete req;
		});

		status = HEADERS_SENT;
	}

	// write string as part of body
	OtObject write(const char* text, size_t size) {
		if (state == START) {
			sendHeaders();
		}

		// send body
		uv_buf_t buffer = uv_buf_init(strndup(text, size), size);
		uv_write_t* uv_write_req = new uv_write_t;
		uv_write_req->data = buffer.base;

		uv_write(uv_write_req, clientStream, &buffer, 1, [](uv_write_t* req, int status) {
			delete (char*) (req->data);
			delete req;
		});

		return getSharedPtr();
	}

	OtObject write(const std::string& text) {
		return write(text.c_str(), text.size());
	}

	// end the response
	OtObject end() {
		if (state == START) {
			headers.emplace("Content-Type", "text/plain");
			headers.emplace("Content-Length", std::to_string(explanation.size()));
			write(explanation);
		}

		status = COMPLETE;
		return getSharedPtr();
	}

	// send body
	OtObject send(const std::string& text) {
		if (!hasHeader("Content-Type")) {
			headers.emplace("Content-Type", "text/plain");
		}

		headers.emplace("Content-Length", std::to_string(text.size()));
		write(text);
		end();

		return getSharedPtr();
	};

	// send result as jSON
	OtObject json(OtObject object) {
		setStatus(200);
		std::string text = object->repr();
		headers.emplace("Content-Type", "application/json");
		headers.emplace("Content-Length", std::to_string(text.size()));
		write(text);
		end();

		return getSharedPtr();
	}

	// send a file as the response
	OtObject sendfile(const std::string& name) {
		std::filesystem::file_status status = std::filesystem::status(name);

		// handle file not found error
		if (!std::filesystem::is_regular_file(status)) {
			setStatus(404);
			end();

		} else {
			// set status
			setStatus(200);

			// set headers
			std::filesystem::path path(name);
			headers.emplace("Content-Length", std::to_string(std::filesystem::file_size(name)));
			headers.emplace("Content-Type", OtMimeTypeGet(path.extension().string().substr(1, std::string::npos)));

			// create pipe to file
			uv_pipe_init(uv_default_loop(), &fileStream, 0);
			auto fd = uv_fs_open(uv_default_loop(), &file_req, name.c_str(), O_RDONLY, 0, nullptr);
			uv_pipe_open(&fileStream, fd);

			fileStream.data = this;

			// allocate memory and attempt to read file
			auto status = uv_read_start(
				(uv_stream_t*) &fileStream,
				[](uv_handle_t* handle, size_t size, uv_buf_t* buffer) {
					*buffer = uv_buf_init((char*) malloc(size), size);
				},
				[](uv_stream_t* stream, ssize_t nread, const uv_buf_t* buffer) {
					((OtHttpResponseClass*)(stream->data))->onFileRead(buffer, nread);
				});

			UV_CHECK_ERROR("uv_read_start", status);
		}

		return getSharedPtr();
	}

	// download user file
	OtObject download(const std::string& name) {
		std::filesystem::path path(name);
		headers.emplace("Content-Disposition", "attachment; filename=" + path.filename().string());
		return sendfile(name);
	}

	// handle file read events
	void onFileRead(const uv_buf_t* buffer, ssize_t nread) {
		if (nread > 0) {
			// write file to socket
			write(buffer->base, nread);

		} else if (nread == UV_EOF) {
			uv_close((uv_handle_t*) &fileStream, nullptr);
			end();

		} else {
			UV_CHECK_ERROR("uv_read_start", nread);
		}

		// free buffer data
		free(buffer->base);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtHttpResponseClass>("HttpResponse", OtInternalClass::getMeta());
			type->set("setStatus", OtFunctionCreate(&OtHttpResponseClass::setStatus));
			type->set("setHeader", OtFunctionCreate(&OtHttpResponseClass::setHeader));
			type->set("hasHeader", OtFunctionCreate(&OtHttpResponseClass::hasHeader));
			type->set("end", OtFunctionCreate(&OtHttpResponseClass::end));
			type->set("send", OtFunctionCreate(&OtHttpResponseClass::send));
			type->set("sendfile", OtFunctionCreate(&OtHttpResponseClass::sendfile));
			type->set("download", OtFunctionCreate(&OtHttpResponseClass::download));
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
	enum OtResponseState {
		START = 1,
		HEADERS_SENT,
		COMPLETE
	};

	OtResponseState state;
	int status;
	std::string explanation;
	OtHttpHeaders headers;

	uv_stream_t* clientStream;
	uv_pipe_t fileStream;
	uv_fs_t file_req;
};
