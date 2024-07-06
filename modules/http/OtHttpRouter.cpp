//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <regex>

#include "OtCallback.h"
#include "OtFunction.h"
#include "OtHttpNext.h"
#include "OtHttpRouter.h"
#include "OtHttpTimer.h"
#include "OtVM.h"


//
//	OtHttpMethodHandler
//

class OtHttpMethodHandler : public OtHttpRouterClass::OtHandler {
public:
	OtHttpMethodHandler(const std::string& m, const std::string& p, OtObject cb) : method(m), path(p), callback(cb) {
		// sanity check
		OtCallbackValidate(callback, 3);

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
				break;

			case STARTSWITH:
				return (method == "ALL" || req->getMethod() == method) && OtText::startsWith(req->getPath(), path);
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

	void run(OtHttpRequest req, OtHttpResponse res, OtObject next) override {
		// execute callback if method and path matches
		if (match(req)) {
			OtVM::instance()->callMemberFunction(callback, "__call__", req, res, next);

		} else {
			// no match, call next handler
			OtVM::instance()->callMemberFunction(next, "__call__");
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
	std::string path;
	std::regex pattern;
	std::vector<std::string> names;
	OtObject callback;
};


//
//	OtStaticHandler
//

class OtStaticHandler : public OtHttpRouterClass::OtHandler {
public:
	OtStaticHandler(const std::string& p, const std::string& f) : serverPath(p), fsPath(f) {}

	void run(OtHttpRequest req, OtHttpResponse res, OtObject next) override {
		if (OtText::startsWith(req->getPath(), serverPath)) {
			// send file
			res->sendfile(fsPath + req->getPath().substr(serverPath.size(), -1));

		} else {
			// no match, pass to next handler
			OtVM::instance()->callMemberFunction(next, "__call__");
		}
	}

private:
	std::string serverPath;
	std::string fsPath;
};


//
//	OtHttpRouterClass::addHandler
//

OtObject OtHttpRouterClass::addHandler(const std::string& method, const std::string& path, OtObject callback) {
	OtCallbackValidate(callback, 3);
	handlers.push_back(std::make_shared<OtHttpMethodHandler>(method, path, callback));
	return OtHttpRouter(this);
}


//
//	OtHttpRouterClass::runHandler
//

void OtHttpRouterClass::runHandler(const size_t index, OtHttpRequest req, OtHttpResponse res, OtObject next) {
	if (index < handlers.size()) {
		handlers[index]->run(req, res, OtHttpNext::create(this, index + 1, req, res, next));

	} else {
		OtVM::instance()->callMemberFunction(next, "__call__");
	}
}


//
//	OtHttpRouterClass::useHandler
//

OtObject OtHttpRouterClass::useHandler(OtObject callback) {
	OtCallbackValidate(callback, 3);
	handlers.push_back(std::make_shared<OtHttpMethodHandler>("", "*", callback));
	return OtHttpRouter(this);
}


//
//	OtHttpRouterClass::allHandler
//

OtObject OtHttpRouterClass::allHandler(const std::string& path, OtObject callback) {
	return addHandler("ALL", path, callback);
}


//
//	OtHttpRouterClass::getHandler
//

OtObject OtHttpRouterClass::getHandler(const std::string& path, OtObject callback) {
	return addHandler("GET", path, callback);
}


//
//	OtHttpRouterClass::putHandler
//

OtObject OtHttpRouterClass::putHandler(const std::string& path, OtObject callback) {
	return addHandler("PUT", path, callback);
}


//
//	OtHttpRouterClass::postHandler
//

OtObject OtHttpRouterClass::postHandler(const std::string& path, OtObject callback) {
	return addHandler("POST", path, callback);
}


//
//	OtHttpRouterClass::deleleteHandler
//

OtObject OtHttpRouterClass::deleleteHandler(const std::string& path, OtObject callback) {
	return addHandler("DELETE", path, callback);
}


//
//	OtHttpRouterClass::staticFiles
//

OtObject OtHttpRouterClass::staticFiles(const std::string& serverPath, const std::string& filePath) {
	handlers.push_back(std::make_shared<OtStaticHandler>(serverPath, filePath));
	return OtHttpRouter(this);
}


//
//	OtHttpRouterClass::call
//

OtObject OtHttpRouterClass::call(OtObject req, OtObject res, OtObject next) {
	runHandler(0, OtHttpRequest(req), OtHttpResponse(res), next);
	return OtHttpRouter(this);
}


//
//	OtHttpRouterClass::timer
//

OtObject OtHttpRouterClass::timer(int64_t wait, int64_t repeat, OtObject callback) {
	OtCallbackValidate(callback, 0);
	return OtHttpTimer::create(wait, repeat, callback);
}


//
//	OtHttpRouterClass::getMeta
//

OtType OtHttpRouterClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtHttpRouterClass>("HttpRouter", OtHttpClass::getMeta());
		type->set("use", OtFunction::create(&OtHttpRouterClass::useHandler));
		type->set("all", OtFunction::create(&OtHttpRouterClass::allHandler));
		type->set("get", OtFunction::create(&OtHttpRouterClass::getHandler));
		type->set("put", OtFunction::create(&OtHttpRouterClass::putHandler));
		type->set("post", OtFunction::create(&OtHttpRouterClass::postHandler));
		type->set("delete", OtFunction::create(&OtHttpRouterClass::deleleteHandler));
		type->set("static", OtFunction::create(&OtHttpRouterClass::staticFiles));
		type->set("timer", OtFunction::create(&OtHttpRouterClass::timer));
		type->set("__call__", OtFunction::create(&OtHttpRouterClass::call));
	}

	return type;
}
