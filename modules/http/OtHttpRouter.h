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
//	OtHttpRouter
//

class OtHttpRouterClass;
typedef std::shared_ptr<OtHttpRouterClass> OtHttpRouter;

class OtHttpRouterClass : public OtHttpClass {
protected:
	// OtHttpNotFound
	class OtHttpNotFoundClass;
	typedef std::shared_ptr<OtHttpNotFoundClass> OtHttpNotFound;

	class OtHttpNotFoundClass : public OtInternalClass {
	public:
		OtHttpNotFoundClass() = default;
		OtHttpNotFoundClass(OtHttpResponse s) : res(s) {}

		// execute not found
		void call() {
			// nobody wanted the request so it must be a case of "Resource Not Found"
			res->setStatus(404);
			res->end();
		}

		// get type definition
		static OtType getMeta() {
			static OtType type = nullptr;

			if (!type) {
				type = OtTypeClass::create<OtHttpNotFoundClass>("HttpNotFound", OtInternalClass::getMeta());
				type->set("__call__", OtFunctionCreate(&OtHttpNotFoundClass::call));
			}

			return type;
		}

		// create a new object
		static OtHttpNotFound create(OtHttpResponse s) {
			OtHttpNotFound notfound = std::make_shared<OtHttpNotFoundClass>(s);
			notfound->setType(getMeta());
			return notfound;
		}

	private:
		OtHttpResponse res;
	};

private:
	// OtHttpNotFound
	class OtHttpNextClass;
	typedef std::shared_ptr<OtHttpNextClass> OtHttpNext;

	class OtHttpNextClass : public OtInternalClass {
	public:
		OtHttpNextClass() = default;
		OtHttpNextClass(OtHttpRouter r, size_t i, OtHttpRequest q, OtHttpResponse s, OtObject n)
			: router(r), index(i), req(q), res(s), next(n) {}

		// execute next
		void call() {
			router->runHandler(index, req, res, next);
		}

		// get type definition
		static OtType getMeta() {
			static OtType type = nullptr;

			if (!type) {
				type = OtTypeClass::create<OtHttpNextClass>("HttpNext", OtInternalClass::getMeta());
				type->set("__call__", OtFunctionCreate(&OtHttpNextClass::call));
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
		OtHttpRouter router;
		size_t index;
		OtHttpRequest req;
		OtHttpResponse res;
		OtObject next;
	};

	class OtHandler {
	public:
		OtHandler(const std::string& m, OtObject c, const std::string& p, OtObject b) : method(m), context(c), path(p), callback(b) {
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

		OtHttpHandlerType type;
		std::string method;
		OtObject context;
		std::string path;
		std::regex pattern;
		std::vector<std::string> names;
		OtObject callback;
	};

private:
	// add handler
	void add(const std::string& method, OtObject context, size_t count, OtObject* parameters) {
		if (count != 2) {
			OT_EXCEPT("Function expects 2 parameters, %d given", count);
		}

		handlers.push_back(OtHandler(method, context, (std::string) (*parameters[0]), parameters[1]));
	}

	// run specified handler
	void runHandler(const size_t index, OtHttpRequest req, OtHttpResponse res, OtObject next) {
		if (index < handlers.size()) {
			handlers[index].run(req, res, OtHttpNextClass::create(cast<OtHttpRouterClass>(), index + 1, req, res, next));

		} else {
			next->get("__call__")->operator ()(getSharedPtr(), 1, &next);
		}
	}

public:
	// add handlers
	OtObject use(OtObject context, size_t count, OtObject* parameters) {
		if (count != 1) {
			OT_EXCEPT("Use function expects 1 parameters, %d given", count);
		}

		handlers.push_back(OtHandler("", context, "*", parameters[0]));
		return getSharedPtr();
	}

	OtObject all(OtObject context, size_t count, OtObject* parameters) {
		add("ALL", context, count, parameters);
		return getSharedPtr();
	}

	OtObject get(OtObject context, size_t count, OtObject* parameters) {
		add("GET", context, count, parameters);
		return getSharedPtr();
	}

	OtObject put(OtObject context, size_t count, OtObject* parameters) {
		add("PUT", context, count, parameters);
		return getSharedPtr();
	}

	OtObject post(OtObject context, size_t count, OtObject* parameters) {
		add("POST", context, count, parameters);
		return getSharedPtr();
	}

	OtObject del(OtObject context, size_t count, OtObject* parameters) {
		add("DELETE", context, count, parameters);
		return getSharedPtr();
	}

	void call(OtObject req, OtObject res, OtObject next) {
		runHandler(0, req->cast<OtHttpRequestClass>(), res->cast<OtHttpResponseClass>(), next);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtHttpRouterClass>("HttpRouter", OtHttpClass::getMeta());
			type->set("use", OtFunctionClass::create(&OtHttpRouterClass::use));
			type->set("all", OtFunctionClass::create(&OtHttpRouterClass::all));
			type->set("get", OtFunctionClass::create(&OtHttpRouterClass::get));
			type->set("put", OtFunctionClass::create(&OtHttpRouterClass::put));
			type->set("post", OtFunctionClass::create(&OtHttpRouterClass::post));
			type->set("delete", OtFunctionClass::create(&OtHttpRouterClass::del));
			type->set("__call__", OtFunctionCreate(&OtHttpRouterClass::call));
		}

		return type;
	}

	// create a new object
	static OtHttpRouter create() {
		OtHttpRouter Router = std::make_shared<OtHttpRouterClass>();
		Router->setType(getMeta());
		return Router;
	}

private:
	std::vector<OtHandler> handlers;
};
