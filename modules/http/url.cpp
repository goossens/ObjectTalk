//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <regex>

#include "ot/exception.h"
#include "ot/function.h"

#include "url.h"


//
//	OtURLClass::init
//

OtObject OtURLClass::init(size_t count, OtObject* parameters) {
	if (count != 1) {
		OtExcept("URL initializer expected 1 parameter not [%d]", count);
	}

	parse(parameters[0]->operator std::string());
	return shared();
}


//
//	submatch
//

static std::string submatch(const std::smatch& match, int index) {
	auto& sub = match[index];
	return std::string(sub.first, sub.second);
}


//
//	OtURLClass::parse
//

void OtURLClass::parse(const std::string& url) {
	this->url = url;

	static std::regex urlRegex(
		"([a-zA-Z][a-zA-Z0-9+.-]*):"	// scheme:
		"([^?#]*)"						// authority and path
		"(?:\\?([^#]*))?"				// ?query
		"(?:#(.*))?");					// #fragment

	std::smatch match;

	if (!std::regex_match(url, match, urlRegex)) {
		OtExcept("Invalid URL [%s]", url.c_str());
	}

	scheme = submatch(match, 1);
	std::transform(scheme.begin(), scheme.end(), scheme.begin(), ::tolower);
	path = submatch(match, 2);

	static std::regex authorityAndPathRegex("//([^/]*)(/.*)?");
	std::smatch authorityAndPathMatch;

	if (std::regex_match(path, authorityAndPathMatch, authorityAndPathRegex)) {
		authority = submatch(authorityAndPathMatch, 1);

		static std::regex authorityRegex(
			"(?:([^@:]*)(?::([^@]*))?@)?"	// username, password
			"(\\[[^\\]]*\\]|[^\\[:]*)"		// host (IP-literal (e.g. '['+IPv6+']', dotted-IPv4, or named host)
			"(?::(\\d*))?");				// port

		std::smatch authorityMatch;

		if (!std::regex_match(authority, authorityMatch, authorityRegex)) {
			OtExcept("Invalid URL authority [%s]", authority.c_str());
		}

		username = submatch(authorityMatch, 1);
		password = submatch(authorityMatch, 2);
		host = submatch(authorityMatch, 3);
		auto portString = submatch(authorityMatch, 4);
		port = (portString.size()) ? std::stoi(portString) : 0;

		path = submatch(authorityAndPathMatch, 2);

		auto slash = path.rfind('/');

		if (slash == std::string::npos) {
			directory ="";
			filename = path;

		} else {
			directory = path.substr(0, slash);
			filename = path.substr(slash + 1);
		}

		auto period = filename.rfind('.');

		if (period == std::string::npos) {
			stem = filename;
			extension ="";

		} else {
			stem = filename.substr(0, period);
			extension = filename.substr(period + 1);
			std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
		}
	}

	query = submatch(match, 3);

	if (query.size()) {
		static std::regex queryParamRegex(
			"(^|&)"			// start of query or start of parameter "&"
			"([^=&]*)=?"	// parameter name and "=" if value is expected
			"([^=&]*)"		// parameter value
			"(?=(&|$))");	// next should be end of query or start of next parameter

		for (auto i = std::sregex_iterator(query.begin(), query.end(), queryParamRegex); i != std::sregex_iterator(); i++) {
			queryParams[OtText::decodeURL(std::string((*i)[2].first, (*i)[2].second))] =
				OtText::decodeURL(std::string((*i)[3].first, (*i)[3].second));
		}
	}

	fragment = submatch(match, 4);
}


//
//	OtURLClass::getMeta
//

OtType OtURLClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtURLClass>("URL", OtHttpClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtURLClass::init));

		type->set("url", OtFunctionClass::create(&OtURLClass::getURL));
		type->set("scheme", OtFunctionClass::create(&OtURLClass::getScheme));
		type->set("authority", OtFunctionClass::create(&OtURLClass::getAuthority));
		type->set("user", OtFunctionClass::create(&OtURLClass::getUsername));
		type->set("password", OtFunctionClass::create(&OtURLClass::getPassword));
		type->set("host", OtFunctionClass::create(&OtURLClass::getHost));
		type->set("port", OtFunctionClass::create(&OtURLClass::getPort));
		type->set("path", OtFunctionClass::create(&OtURLClass::getPath));
		type->set("directory", OtFunctionClass::create(&OtURLClass::getDirectory));
		type->set("filename", OtFunctionClass::create(&OtURLClass::getFilename));
		type->set("stem", OtFunctionClass::create(&OtURLClass::getStem));
		type->set("extension", OtFunctionClass::create(&OtURLClass::getExtension));
		type->set("query", OtFunctionClass::create(&OtURLClass::getQuery));
		type->set("fragment", OtFunctionClass::create(&OtURLClass::getFragment));

		type->set("hasParam", OtFunctionClass::create(&OtURLClass::hasParam));
		type->set("getParam", OtFunctionClass::create(&OtURLClass::getParam));
		type->set("getParamWithDefault", OtFunctionClass::create(&OtURLClass::getParamWithDefault));
	}

	return type;
}


//
//	OtURLClass::create
//

OtURL OtURLClass::create(const std::string& value) {
	OtURL Path = std::make_shared<OtURLClass>(value);
	Path->setType(getMeta());
	return Path;
}
