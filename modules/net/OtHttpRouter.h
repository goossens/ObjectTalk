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


//
//	OtHttpRouterClass
//

class OtHttpRouterClass : public OtNetClass {
private:
	class OtHttpNextClass : public OtInternalClass {
	public:
		OtHttpNextClass() = default;
		OtHttpNextClass(OtHttpRouter r, size_t i, OtHttpRequest q, OtHttpResponse s) : router(r), index(i), req(q), res(s) {}

		// execute next
		void next() {
			router->runHandler(index, req, res);
		}

		// get type definition
		static OtType getMeta() {
			static OtType type = nullptr;

			if (!type) {
				type = OtTypeClass::create<OtHttpNextClass>("HttpNext", OtInternalClass::getMeta());
				type->set("__call__", OtFunctionCreate(&OtHttpNextClass::next));
			}

			return type;
		}

		// create a new object
		static OtObject create(OtHttpRouter r, size_t i, OtHttpRequest q, OtHttpResponse s) {
			return std::make_shared<OtHttpNextClass>(r, i, q, s)->setType(getMeta());
		}

	private:
		OtHttpRouter router;
		size_t index;
		OtHttpRequest req;
		OtHttpResponse res;
	};

	class OtHandler {
	public:
		OtHandler(const std::string& m, OtObject c, const std::string& p, OtObject b) : method(m), context(c), path(p), callback(b) {
			if (method.length() == 0) {
				type = ALL;

			} else if (path.find('*') != std::string::npos) {
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
				case ALL:
					return true;
					break;

				case MATCH:
					return req->getMethod() == method && req->getPath() == path;
					return true;
					break;

				case STARTSWITH:
					return req->getMethod() == method && req->getPath().rfind(path, 0) == 0;
					break;

				case REGEX:
					if (req->getMethod() == method) {
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
				callback->get("__call__")->operator ()(context, 3, pars);

			} else {
				// no match, call next handler
				next->cast<OtHttpNextClass>()->next();
			}
		}

	private:
		enum OtHandlerType {
			ALL = 1,
			MATCH,
			STARTSWITH,
			REGEX
		};

		OtHandlerType type;
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
	void runHandler(const size_t index, OtHttpRequest req, OtHttpResponse res) {
		if (index < handlers.size()) {
			handlers[index].run(req, res, OtHttpNextClass::create(cast<OtHttpRouterClass>(), index + 1, req, res));
		}
	}

public:
	// add handlers
	OtObject use(OtObject context, size_t count, OtObject* parameters) {
		add("", context, count, parameters);
		return nullptr;
	}

	OtObject get(OtObject context, size_t count, OtObject* parameters) {
		add("GET", context, count, parameters);
		return nullptr;
	}

	OtObject put(OtObject context, size_t count, OtObject* parameters) {
		add("PUT", context, count, parameters);
		return nullptr;
	}

	OtObject post(OtObject context, size_t count, OtObject* parameters) {
		add("POST", context, count, parameters);
		return nullptr;
	}

	OtObject del(OtObject context, size_t count, OtObject* parameters) {
		add("DELETE", context, count, parameters);
		return nullptr;
	}

	void dispatch(OtHttpRequest req, OtHttpResponse res) {
		runHandler(0, req, res);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtHttpRouterClass>("HttpRouter", OtNetClass::getMeta());
			type->set("use", OtFunctionClass::create(&OtHttpRouterClass::use));
			type->set("get", OtFunctionClass::create(&OtHttpRouterClass::get));
			type->set("put", OtFunctionClass::create(&OtHttpRouterClass::put));
			type->set("post", OtFunctionClass::create(&OtHttpRouterClass::post));
			type->set("delete", OtFunctionClass::create(&OtHttpRouterClass::del));
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
