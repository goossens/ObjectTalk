//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"
#include "OtFunction.h"
#include "OtModule.h"

#include "OtHttp.h"
#include "OtHttpRouter.h"
#include "OtHttpServer.h"
#include "OtUrl.h"
#include "OtString.h"


//
//	OtUrlObject
//

class OtUrlClass;
using OtUrlObject = OtObjectPointer<OtUrlClass>;

class OtUrlClass : public OtHttpClass {
public:
	// initialize URL
	void init(size_t count, OtObject* parameters) {
		if (count != 1) {
			OtLogError("URL initializer expected 1 parameter not [{}]", count);
		}

		url.parse(parameters[0]->operator std::string());
	}

	// parse URL string
	void parse(const std::string& u) { url.parse(u); }

	// get URL parts
	operator std::string() { return url.getURL(); }
	const std::string& getURL() { return url.getURL(); }
	const std::string& getScheme() { return url.getScheme(); }
	const std::string& getAuthority() { return url.getAuthority(); }
	const std::string& getUsername() { return url.getUsername(); }
	const std::string& getPassword() { return url.getPassword(); }
	const std::string& getHost() { return url.getHost(); }
	int getPort() { return url.getPort(); }
	const std::string& getPath() { return url.getPath(); }
	const std::string& getDirectory() { return url.getDirectory(); }
	const std::string& getFilename() { return url.getFilename(); }
	const std::string& getStem() { return url.getStem(); }
	const std::string& getExtension() { return url.getExtension(); }
	const std::string& getQuery() { return url.getQuery(); }
	const std::string& getFragment() { return url.getFragment(); }
	bool hasParam(const std::string& p) { return url.hasParam(p); }

	OtObject getParam(const std::string& p) {
		return OtString::create(url.getParam(p));
	}

	OtObject getParamWithDefault(const std::string& p, const OtObject d) {
		return url.hasParam(p) ? getParam(p) : d;
	}

	// download the data
	inline const std::string& download() { return url.download(); }
	inline int getStatus() { return url.getStatus(); }

	// get type definition
	static OtType getMeta() {
		static OtType type;

		if (!type) {
			type = OtType::create<OtUrlClass>("URL", OtHttpClass::getMeta());
			type->set("__init__", OtFunction::create(&OtUrlClass::init));

			type->set("url", OtFunction::create(&OtUrlClass::getURL));
			type->set("scheme", OtFunction::create(&OtUrlClass::getScheme));
			type->set("authority", OtFunction::create(&OtUrlClass::getAuthority));
			type->set("user", OtFunction::create(&OtUrlClass::getUsername));
			type->set("password", OtFunction::create(&OtUrlClass::getPassword));
			type->set("host", OtFunction::create(&OtUrlClass::getHost));
			type->set("port", OtFunction::create(&OtUrlClass::getPort));
			type->set("path", OtFunction::create(&OtUrlClass::getPath));
			type->set("directory", OtFunction::create(&OtUrlClass::getDirectory));
			type->set("filename", OtFunction::create(&OtUrlClass::getFilename));
			type->set("stem", OtFunction::create(&OtUrlClass::getStem));
			type->set("extension", OtFunction::create(&OtUrlClass::getExtension));
			type->set("query", OtFunction::create(&OtUrlClass::getQuery));
			type->set("fragment", OtFunction::create(&OtUrlClass::getFragment));

			type->set("hasParam", OtFunction::create(&OtUrlClass::hasParam));
			type->set("getParam", OtFunction::create(&OtUrlClass::getParam));
			type->set("getParamWithDefault", OtFunction::create(&OtUrlClass::getParamWithDefault));

			type->set("download", OtFunction::create(&OtUrlClass::download));
			type->set("getStatus", OtFunction::create(&OtUrlClass::getStatus));
		}

		return type;
	}

protected:
	// constructors
	friend class OtObjectPointer<OtUrlClass>;
	OtUrlClass() = default;
	OtUrlClass(const std::string& path) { url.parse(path); }

private:
	// properties
	OtUrl url;
};


//
//	Module registration
//

static OtModuleRegistration registration{"http", [](OtModule module) {
	module->set("URL", OtClass::create(OtUrlClass::getMeta()));
	module->set("Router", OtClass::create(OtHttpRouterClass::getMeta()));
	module->set("Server", OtClass::create(OtHttpServerClass::getMeta()));
}};
