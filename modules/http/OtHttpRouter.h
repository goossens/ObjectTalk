//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtHttpRouter
//

class OtHttpRouterClass;
typedef std::shared_ptr<OtHttpRouterClass> OtHttpRouter;

class OtHttpRouterClass : public OtHttpClass {
private:
	//
	// OtHttpNextClass
	//

	class OtHttpNextClass;
	typedef std::shared_ptr<OtHttpNextClass> OtHttpNext;

	class OtHttpNextClass : public OtInternalClass {
	public:
		OtHttpNextClass() = default;
		OtHttpNextClass(OtHttpRouter r, size_t i, OtHttpRequest q, OtHttpResponse s, OtObject n)
			: router(r), index(i), req(q), res(s), next(n) {}

		// execute next
		void call() {
			router.lock()->runHandler(index, req, res, next);
		}

		// get type definition
		static OtType getMeta() {
			static OtType type = nullptr;

			if (!type) {
				type = OtTypeClass::create<OtHttpNextClass>("HttpNext", OtInternalClass::getMeta());
				type->set("__call__", OtFunctionClass::create(&OtHttpNextClass::call));
			}

			return type;
		}

		// create a new object
		static OtHttpNext create(OtHttpRouter r, size_t i, OtHttpRequest q, OtHttpResponse s, OtObject n) {
			OtHttpNext next = std::make_shared<OtHttpNextClass>(r, i, q, s, n);
			next->setType(getMeta());
			return next;
		}

	private:
		std::weak_ptr<OtHttpRouterClass> router;
		size_t index;
		OtHttpRequest req;
		OtHttpResponse res;
		OtObject next;
	};


	//
	//	Abstract request handler class
	//

	class OtHandler {
	public:
		// constructor
		OtHandler() = default;

		// destructor
		virtual ~OtHandler() {}

		// run the handler
		virtual void run(OtHttpRequest req, OtHttpResponse res, OtObject next) {}
	};

	//
	//	Request handler basd on method and path
	//

	class OtMethodHandler : public OtHandler {
	public:
		OtMethodHandler(OtContext c, const std::string& m, const std::string& p, OtObject b) : context(c), method(m), path(p), callback(b) {
			if (method.length() == 0) {
				type = USE;

			} else if (path.back() == '*') {
				type = STARTSWITH;
				path = path.substr(0, path.length() - 1);

			} else if (path.find(':') != std::string::npos) {
				type = REGEX;

				std::regex nameRE(":([^\\/]+)?");
				std::sregex_token_iterator it(path.begin(), path.end(), nameRE);
				std::sregex_token_iterator end;

				while (it != end) {
					std::string name = *it++;
					names.push_back(name.erase(0, 1));
				}

				pattern = std::regex("^" + std::regex_replace(path, nameRE, "(?:([^\\/]+?))") +"$");

			} else {
				type = MATCH;
			}
		}

		bool match(OtHttpRequest req) {
			switch(type) {
				case USE:
					return true;
					break;

				case MATCH:
					return (method == "ALL" || req->getMethod() == method) && req->getPath() == path;
					return true;
					break;

				case STARTSWITH:
					return (method == "ALL" || req->getMethod() == method) && OtTextStartsWith(req->getPath(), path);
					break;

				case REGEX:
					if (method == "ALL" || req->getMethod() == method) {
						std::string path = req->getPath();
						std::smatch values;

						if (regex_match(path, values, pattern)) {
							for (auto i = 1; i < values.size(); i++) {
								req->setParam(names[i], values[i]);
							}

						} else {
							return false;
						}

					} else {
						return false;
					}

					break;
			}

			return false;
		}

		void run(OtHttpRequest req, OtHttpResponse res, OtObject next) {
			// execute callback if method and path matches
			if (match(req)) {
				OtObject pars[] = {callback, req, res, next};
				callback->get("__call__")->operator ()(context, 4, pars);

			} else {
				// no match, call next handler
				next->get("__call__")->operator ()(context, 1, &next);
			}
		}

	private:
		enum OtHttpHandlerType {
			USE = 1,
			MATCH,
			STARTSWITH,
			REGEX
		};

		OtContext context;
		OtHttpHandlerType type;
		std::string method;
		std::string path;
		std::regex pattern;
		std::vector<std::string> names;
		OtObject callback;
	};

	//
	//	Request handler for static files
	//

	class OtStaticHandler : public OtHandler {
	public:
		OtStaticHandler(OtContext c, const std::string& p, const std::string& f) : context(c), serverPath(p), fsPath(f) {}

		void run(OtHttpRequest req, OtHttpResponse res, OtObject next) {
			if (OtTextStartsWith(req->getPath(), serverPath)) {
				// send file
				res->sendfile(fsPath + req->getPath().substr(serverPath.size(), -1));

			} else {
				// no match, pass to next handler
				next->get("__call__")->operator ()(context, 1, &next);
			}
		}

	private:
		OtContext context;
		std::string serverPath;
		std::string fsPath;
	};

private:
	// add handler
	OtObject addHandler(OtContext context, const std::string& method, size_t count, OtObject* parameters) {
		if (count != 2) {
			OT_EXCEPT("%s function expects 2 parameters, %d given", method.c_str(), count);
		}

		handlers.push_back(std::make_shared<OtMethodHandler>(context, method, (std::string) (*parameters[0]), parameters[1]));
		return getSharedPtr();
	}

	// run specified handler
	void runHandler(const size_t index, OtHttpRequest req, OtHttpResponse res, OtObject next) {
		if (index < handlers.size()) {
			handlers[index]->run(req, res, OtHttpNextClass::create(cast<OtHttpRouterClass>(), index + 1, req, res, next));

		} else {
			next->get("__call__")->operator ()(nullptr, 1, &next);
		}
	}

public:
	// constructor
	OtHttpRouterClass() = default;

	// add handlers
	OtObject useHandler(OtContext context, size_t count, OtObject* parameters) {
		if (count != 1) {
			OT_EXCEPT("Use function expects 1 parameters, %d given", count);
		}

		handlers.push_back(std::make_shared<OtMethodHandler>(context, "", "*", parameters[0]));
		return getSharedPtr();
	}

	OtObject allHandler(OtContext context, size_t count, OtObject* parameters) {
		return addHandler(context, "ALL", count, parameters);
	}

	OtObject getHandler(OtContext context, size_t count, OtObject* parameters) {
		return addHandler(context, "GET", count, parameters);
	}

	OtObject putHandler(OtContext context, size_t count, OtObject* parameters) {
		return addHandler(context, "PUT", count, parameters);
	}

	OtObject postHandler(OtContext context, size_t count, OtObject* parameters) {
		return addHandler(context, "POST", count, parameters);
	}

	OtObject deleleteHandler(OtContext context, size_t count, OtObject* parameters) {
		return addHandler(context, "DELETE", count, parameters);
	}

	OtObject staticFiles(OtContext context, size_t count, OtObject* parameters) {
		if (count != 2) {
			OT_EXCEPT("Static function expects 2 parameters, %d given", count);
		}

		std::string serverPath = parameters[0]->operator std::string();
		std::string filePath = parameters[1]->operator std::string();

		handlers.push_back(std::make_shared<OtStaticHandler>(context, serverPath, filePath));
		return getSharedPtr();
	}

	OtObject call(OtContext context, size_t count, OtObject* p) {
		if (count != 3) {
			OT_EXCEPT("HttpRouter expects 3 parameters, %d given", count);
		}

		runHandler(0, p[0]->cast<OtHttpRequestClass>(), p[1]->cast<OtHttpResponseClass>(), p[2]);
		return getSharedPtr();
	}

	void clearHandlers() {
		handlers.clear();
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtHttpRouterClass>("HttpRouter", OtHttpClass::getMeta());
			type->set("use", OtFunctionClass::create(&OtHttpRouterClass::useHandler));
			type->set("all", OtFunctionClass::create(&OtHttpRouterClass::allHandler));
			type->set("get", OtFunctionClass::create(&OtHttpRouterClass::getHandler));
			type->set("put", OtFunctionClass::create(&OtHttpRouterClass::putHandler));
			type->set("post", OtFunctionClass::create(&OtHttpRouterClass::postHandler));
			type->set("delete", OtFunctionClass::create(&OtHttpRouterClass::deleleteHandler));
			type->set("static", OtFunctionClass::create(&OtHttpRouterClass::staticFiles));
			type->set("__call__", OtFunctionClass::create(&OtHttpRouterClass::call));
			type->set("clearHandlers", OtFunctionClass::create(&OtHttpRouterClass::clearHandlers));
		}

		return type;
	}

	// create a new object
	static OtHttpRouter create() {
		OtHttpRouter router = std::make_shared<OtHttpRouterClass>();
		router->setType(getMeta());
		return router;
	}

private:
	std::vector<std::shared_ptr<OtHandler>> handlers;
};
