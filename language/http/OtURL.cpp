//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cctype>
#include <regex>

#include "fmt/format.h"
#include "httplib.h"

#include "OtLog.h"
#include "OtText.h"
#include "OtUrl.h"


//
//	submatch
//

static std::string submatch(const std::smatch& match, int index) {
	auto& sub = match[index];
	return std::string(sub.first, sub.second);
}


//
//	OtUrl::parse
//

static char myToLower(char ch) {
    return static_cast<char>(std::tolower(static_cast<int>(ch)));
}

void OtUrl::parse(const std::string& urlString) {
	url = urlString;

	static std::regex urlRegex(
		"([a-zA-Z][a-zA-Z0-9+.-]*):"	// scheme:
		"([^?#]*)"						// authority and path
		"(?:\\?([^#]*))?"				// ?query
		"(?:#(.*))?");					// #fragment

	std::smatch match;

	if (!std::regex_match(url, match, urlRegex)) {
		OtLogError("Invalid URL [{}]", url);
	}

	scheme = submatch(match, 1);
	std::transform(scheme.begin(), scheme.end(), scheme.begin(), myToLower);
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
			OtLogError("Invalid URL authority [{}]", authority);
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
			std::transform(extension.begin(), extension.end(), extension.begin(), myToLower);
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
//	OtUrl::doGet
//

void OtUrl::doGet() {
	auto schemeHostPort = fmt::format("{}://{}", scheme, host);

	if (port != 80) {
		schemeHostPort += fmt::format(":{}", port);

	}

	httplib::Headers headers = {
		{ "User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:137.0) Gecko/20100101 Firefox/137.0" }
	};

	httplib::Client client(schemeHostPort);
	auto result = client.Get(path, headers);
	status = result->status;

	if (status == httplib::StatusCode::OK_200) {
		data = result->body;

	} else {
		data.clear();
		OtLogError("Can't get [{}]: {}", url, httplib::status_message(status));
	}
}