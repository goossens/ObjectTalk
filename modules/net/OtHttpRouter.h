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
	class OtHandler {
	public:
		OtHandler(const std::wstring& m, const std::wstring& p, OtObject c) : method(m), path(p), callback(c) {
			if (method.length() == 0) {
				type = ALL;

			} else if (path.find('*') != std::string::npos) {
				type = STARTSWITH;
				path = path.substr(0, path.length() - 1);

			} else if (path.find(':') != std::string::npos) {
				type = REGEX;

				std::wregex nameRE(L":([^\\/]+)?");
				std::wsregex_token_iterator it(path.begin(), path.end(), nameRE);
				std::wsregex_token_iterator end;

				while (it != end) {
					std::wstring name = *it++;
					names.push_back(name.erase(0, 1));
				}

				pattern = std::wregex(L"^" + std::regex_replace(path, nameRE,  L"(?:([^\\/]+?))") + L"$");

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
						std::wstring path = req->getPath();
						std::wsmatch values;

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
			if (match(req)) {

			} else {

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
		std::wstring method;
		std::wstring path;
		std::wregex pattern;
		std::vector<std::wstring> names;
		OtObject callback;
	};

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
				type = OtTypeClass::create<OtHttpNextClass>(L"HttpNext", OtInternalClass::getMeta());
				type->set(L"__call__", OtFunctionCreate(&OtHttpNextClass::next));
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

private:
	// add handler
	void add(const std::wstring& method, const std::wstring& path, OtObject callback) {
		handlers.push_back(OtHandler(method, path, callback));
	}

	// run specified handler
	void runHandler(const size_t index, OtHttpRequest req, OtHttpResponse res) {
		if (index < handlers.size()) {
			handlers[index].run(req, res, OtHttpNextClass::create(cast<OtHttpRouterClass>(), index + 1, req, res));
		}
	}

public:
	// add handlers
	void use(const std::wstring& pattern, OtObject callback) {
		add(L"", pattern, callback);
	}

	void get(const std::wstring& pattern, OtObject callback) {
		add(L"GET", pattern, callback);
	}

	void put(const std::wstring& pattern, OtObject callback) {
		add(L"PUT", pattern, callback);
	}

	void post(const std::wstring& pattern, OtObject callback) {
		add(L"POST", pattern, callback);
	}

	void del(const std::wstring& pattern, OtObject callback) {
		add(L"DELETE", pattern, callback);
	}

	void dispatch(OtHttpRequest req, OtHttpResponse res) {
		runHandler(0, req, res);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtHttpRouterClass>(L"HttpRouter", OtNetClass::getMeta());
			type->set(L"use", OtFunctionCreate(&OtHttpRouterClass::use));
			type->set(L"get", OtFunctionCreate(&OtHttpRouterClass::get));
			type->set(L"put", OtFunctionCreate(&OtHttpRouterClass::put));
			type->set(L"post", OtFunctionCreate(&OtHttpRouterClass::post));
			type->set(L"delete", OtFunctionCreate(&OtHttpRouterClass::del));
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
