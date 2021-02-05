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


#include "llhttp/llhttp.h"


//
//	Defines
//

#define UV_CHECK_ERROR(action, status) if (status) OT_EXCEPT("libuv error in %s: %s", action, uv_strerror(status))


//
//	OtHTTP
//

class OtHTTPClass;
typedef std::shared_ptr<OtHTTPClass> OtHTTP;


//
//	OtHTTPClass
//

class OtHTTPClass : public OtHttpRouterClass {
private:
	// class to handle sessions
	class OtHttpSession {
	public:
		OtHttpSession(OtHTTPClass* h) : http(h) {
			// setup request/response objects
			request = OtHttpRequestClass::create();
			response = OtHttpResponseClass::create();

			// setup parser settings
			llhttp_settings_init(&settings);

			// callback for start of message
			settings.on_message_begin = [](llhttp_t* parser) -> int {
				OtHttpSession* session = (OtHttpSession*)(parser->data);
				session->request->clear();
				session->response->clear();
				return HPE_OK;
			};

			// callback for parsed path
			settings.on_url = [](llhttp_t* parser, const char *at, size_t length) -> int {
				((OtHttpSession*)(parser->data))->request->setPath(OtTextFromPtr(at, length));
				return HPE_OK;
			};

			// callback for header name
			settings.on_header_field = [](llhttp_t* parser, const char *at, size_t length) -> int {
				((OtHttpSession*)(parser->data))->header = OtTextFromPtr(at, length);
				return HPE_OK;
			};

			// callback for header value
			settings.on_header_value = [](llhttp_t* parser, const char *at, size_t length) -> int {
				OtHttpSession* session = (OtHttpSession*)(parser->data);
				session->request->setHeader(session->header, OtTextFromPtr(at, length));
				return HPE_OK;
			};

			// callback for complete header state
			settings.on_headers_complete = [](llhttp_t* parser) -> int {
				OtHttpSession* session = (OtHttpSession*)(parser->data);
				session->request->setMethod(OtTextFromPtr(llhttp_method_name((llhttp_method_t) parser->method)));
				session->request->setVersion(OtFormat(L"HTTP/%d.%d", parser->http_major, parser->http_minor));
				return HPE_OK;
			};

			// callback for body content
			settings.on_body = [](llhttp_t* parser, const char *at, size_t length) -> int {
				((OtHttpSession*)(parser->data))->request->appendBody(at, length);
				return HPE_OK;
			};

			// callback for complete messages
			settings.on_message_complete = [](llhttp_t* parser) -> int {
				// dispatch request
				OtHttpSession* session = (OtHttpSession*)(parser->data);
				session->http->dispatch(session->request, session->response);

				// add required headers
				if (session->request->headerIs(L"Connection", L"close")) {
					session->response->setHeader(L"Connection", L"close");

				} else {
					session->response->setHeader(L"Keep-Alive", L"timeout=5, max=5");
				}

				if (!session->response->hasHeader(L"Content-Type")) {
					session->response->setHeader(L"Content-Type", L"text/plain");
				}

				if (!session->response->hasHeader(L"Content-Length")) {
					session->response->setHeader(L"Content-Length", std::to_wstring(session->response->getBodySize()));
				}

				// convert response to stream
				std::stringstream* stream = new std::stringstream();
				session->response->toStream(*stream);

				// send response
				uv_write_t* uv_write_req = new uv_write_t;
				uv_write_req->data = stream;

				std::string str = stream->str();
				uv_buf_t buffer{};
				buffer.base = (char*) str.c_str();
				buffer.len = str.size();

				uv_write(uv_write_req, (uv_stream_t*) &(session->uv_client), &buffer, 1, [](uv_write_t* req, int status) {
					delete (std::stringstream*) (req->data);
					delete req;
				});

				return HPE_OK;
			};

			// setup parser
			llhttp_init(&parser, HTTP_BOTH, &settings);
			parser.data = this;

			// setup client socket
			uv_tcp_init(http->uv_loop, &uv_client);
			uv_client.data = this;

			int status;
			status = uv_accept((uv_stream_t*) &http->uv_server, (uv_stream_t*) &uv_client);
			UV_CHECK_ERROR("uv_accept", status);

			// allocate memory and attempt to read
			status = uv_read_start(
				(uv_stream_t*) &uv_client,
				[](uv_handle_t* handle, size_t size, uv_buf_t* buffer) {
					*buffer = uv_buf_init((char*) malloc(size), size);
				},
				[](uv_stream_t* socket, ssize_t nread, const uv_buf_t* buffer) {
					((OtHttpSession*)(socket->data))->onRead(buffer, nread);
				});

			UV_CHECK_ERROR("uv_read_start", status);
		}

		// handle socket read events
		void onRead(const uv_buf_t* buffer, ssize_t nread) {
			if (nread >= 0) {
				auto status = llhttp_execute(&parser, buffer->base, nread);

			} else if (nread == UV_EOF) {
				uv_close((uv_handle_t*) &uv_client, [](uv_handle_t* handle) {
					delete (OtHttpSession*)(handle->data);
				});

			} else {
				OT_EXCEPT("llhttp error in llhttp_execute: %s", llhttp_errno_name((llhttp_errno_t) nread));
			}

			// free buffer data
			free(buffer->base);
		}

	private:
		OtHttpRequest request;
		OtHttpResponse response;
		std::wstring header;

		OtHTTPClass* http;
		uv_tcp_t uv_client;
		llhttp_settings_t settings;
		llhttp_t parser;
	};

	// handle connection requests
	void onConnect() {
		// create new session (object gets deleted at EOF in read function above)
		new OtHttpSession(this);
	}

public:
	// listen for requests
	void listen(OtObject loop, const std::wstring& ip, long port) {
		uv_loop = loop->cast<OtLoopClass>()->getLoop();
		uv_tcp_init(uv_loop, &uv_server);
		uv_server.data = (void*) this;

		int status;
		struct sockaddr_in address;

		status = uv_ip4_addr(OtTextToNarrow(ip).c_str(), port, &address);
		UV_CHECK_ERROR("uv_ip4_addr", status);

		status = uv_tcp_bind(&uv_server, (const struct sockaddr*) &address, 0);
		UV_CHECK_ERROR("uv_tcp_bind", status);

		status = uv_listen((uv_stream_t*) &uv_server, 128, [](uv_stream_t* socket, int status) {
			UV_CHECK_ERROR("uv_listen", status);
			((OtHTTPClass*)(socket->data))->onConnect();
		});

		UV_CHECK_ERROR("uv_listen", status);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtHTTPClass>(L"HTTP", OtHttpRouterClass::getMeta());
			type->set(L"listen", OtFunctionCreate(&OtHTTPClass::listen));
		}

		return type;
	}

	// create a new object
	static OtHTTP create() {
		OtHTTP http = std::make_shared<OtHTTPClass>();
		http->setType(getMeta());
		return http;
	}

private:
	uv_loop_t* uv_loop;
	uv_tcp_t uv_server;
};
