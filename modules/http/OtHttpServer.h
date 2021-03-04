//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


///
//	OtHTTP
//

class OtHttpServerClass;
typedef std::shared_ptr<OtHttpServerClass> OtHttpServer;

class OtHttpServerClass : public OtHttpRouterClass {
private:
	// class to handle sessions
	class OtHttpSession {
	public:
		OtHttpSession(OtHttpServerClass* s) : server(s) {
			// setup request/response objects
			request = OtHttpRequestClass::create();
			response = OtHttpResponseClass::create();

			// setup parser settings
			llhttp_settings_init(&settings);

			// callback for start of message
			settings.on_message_begin = [](llhttp_t* parser) -> int {
				((OtHttpSession*)(parser->data))->onBegin();
				return HPE_OK;
			};

			// callback for request URL
			settings.on_url = [](llhttp_t* parser, const char *at, size_t length) -> int {
				((OtHttpSession*)(parser->data))->request->onURL(at, length);
				return HPE_OK;
			};

			// callback for header name
			settings.on_header_field = [](llhttp_t* parser, const char *at, size_t length) -> int {
				((OtHttpSession*)(parser->data))->request->onHeaderField(at, length);
				return HPE_OK;
			};

			// callback for header value
			settings.on_header_value = [](llhttp_t* parser, const char *at, size_t length) -> int {
				((OtHttpSession*)(parser->data))->request->onHeaderValue(at, length);
				return HPE_OK;
			};

			// callback for complete header state
			settings.on_headers_complete = [](llhttp_t* parser) -> int {
				((OtHttpSession*)(parser->data))->request->onHeadersComplete(
					std::string(llhttp_method_name((llhttp_method_t) parser->method)),
					OtFormat("HTTP/%d.%d", parser->http_major, parser->http_minor));

				return HPE_OK;
			};

			// callback for body content
			settings.on_body = [](llhttp_t* parser, const char *at, size_t length) -> int {
				((OtHttpSession*)(parser->data))->request->onBody(at, length);
				return HPE_OK;
			};

			// callback for complete messages
			settings.on_message_complete = [](llhttp_t* parser) -> int {
				((OtHttpSession*)(parser->data))->onMessageComplete();
				return HPE_OK;
			};

			// setup HTTP parser
			llhttp_init(&parser, HTTP_BOTH, &settings);
			parser.data = this;

			// setup client socket
			uv_tcp_init(uv_default_loop(), &uv_client);
			uv_client.data = this;

			int status = uv_accept((uv_stream_t*) &server->uv_server, (uv_stream_t*) &uv_client);
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

			// pass client socket to response object for writes
			response->setStream((uv_stream_t*) &(uv_client));
		}

		// event handlers
		void onBegin() {
			request->clear();
			response->clear();
		}

		void onMessageComplete() {
			// finish request
			request->onMessageComplete();

			// set default response headers
			if (request->headerIs("Connection","close")) {
				response->setHeader("Connection","close");

			} else {
				response->setHeader("Keep-Alive","timeout=5, max=5");
			}

			// dispatch request
			server->call(request, response, OtHttpNotFoundClass::create(response));
		}

		// handle socket read events
		void onRead(const uv_buf_t* buffer, ssize_t nread) {
			if (nread >= 0) {
				auto parsed = llhttp_execute(&parser, buffer->base, nread);

				if (parsed != nread) {
					uv_close((uv_handle_t*) &uv_client, [](uv_handle_t* handle) {
						delete (OtHttpSession*)(handle->data);
					});
				}

			} else if (nread == UV_EOF) {
				uv_close((uv_handle_t*) &uv_client, [](uv_handle_t* handle) {
					delete (OtHttpSession*)(handle->data);
				});

			} else {
				throw OtException(OtFormat("llhttp error in llhttp_execute: %s", llhttp_errno_name((llhttp_errno_t) nread)));
			}

			// free buffer data
			free(buffer->base);
		}

	private:
		OtHttpRequest request;
		OtHttpResponse response;

		OtHttpServerClass* server;
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
	// listen for requests on specified IP address and port
	OtObject listen(const std::string& ip, long port) {
		uv_tcp_init(uv_default_loop(), &uv_server);
		uv_server.data = (void*) this;

		int status;
		struct sockaddr_in address;

		status = uv_ip4_addr(ip.c_str(), port, &address);
		UV_CHECK_ERROR("uv_ip4_addr", status);

		status = uv_tcp_bind(&uv_server, (const struct sockaddr*) &address, 0);
		UV_CHECK_ERROR("uv_tcp_bind", status);

		status = uv_listen((uv_stream_t*) &uv_server, 128, [](uv_stream_t* socket, int status) {
			UV_CHECK_ERROR("uv_listen", status);
			((OtHttpServerClass*)(socket->data))->onConnect();
		});

		UV_CHECK_ERROR("uv_listen", status);
		return getSharedPtr();
	}

	// run server
	void run() {
		uv_run(uv_default_loop(), UV_RUN_DEFAULT);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtHttpServerClass>("HttpServer", OtHttpRouterClass::getMeta());
			type->set("listen", OtFunctionCreate(&OtHttpServerClass::listen));
			type->set("run", OtFunctionCreate(&OtHttpServerClass::run));
		}

		return type;
	}

	// create a new object
	static OtHttpServer create() {
		OtHttpServer server = std::make_shared<OtHttpServerClass>();
		server->setType(getMeta());
		return server;
	}

private:
	uv_tcp_t uv_server;
};
