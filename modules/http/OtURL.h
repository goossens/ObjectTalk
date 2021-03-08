//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtURL
//

class OtURLClass;
typedef std::shared_ptr<OtURLClass> OtURL;


//
//	OtURLClass
//

class OtURLClass : public OtHttpClass
{
public:
	// constructors
	OtURLClass() = default;
	OtURLClass(const std::string& path) { parse(path); }

	// initialize URL
	OtObject init(OtContext, size_t count, OtObject* parameters) {
		if (count != 1) {
			throw OtException(OtFormat("URL initializer expected 1 parameter not [%d]", count));
		}

		parse(parameters[0]->operator std::string());
		return getSharedPtr();
	}

	// get URL parts
	operator std::string() { return url; }
	const std::string& getURL() { return url; }
	const std::string& getScheme() { return scheme; }
	const std::string& getAuthority() { return authority; }
	const std::string& getUsername() { return username; }
	const std::string& getPassword() { return password; }
	const std::string& getHost() { return host; }
	const long getPort() { return port; }
	const std::string& getPath() { return path; }
	const std::string& getDirectory() { return directory; }
	const std::string& getFilename() { return filename; }
	const std::string& getStem() { return stem; }
	const std::string& getExtension() { return extension; }
	const std::string& getQuery() { return query; }
	const std::string& getFragment() { return fragment; }

	const bool hasParam(const std::string& p) {
		return queryParams.find(p) != queryParams.end();
	}

	const OtObject getParam(const std::string& p) {
		return OtStringClass::create(queryParams.find(p)->second);
	}

	const OtObject getParamWithDefault(const std::string& p, const OtObject d) {
		return hasParam(p) ? getParam(p) : d;
	}

	const std::unordered_map<std::string, std::string> getParams() {
		return queryParams;
	}

	// parse URL string
	void parse(const std::string& url) {
		this->url = url;

		static std::regex urlRegex(
			"([a-zA-Z][a-zA-Z0-9+.-]*):"	// scheme:
			"([^?#]*)"						// authority and path
			"(?:\\?([^#]*))?"				// ?query
			"(?:#(.*))?");					// #fragment

		std::smatch match;

		if (!std::regex_match(url, match, urlRegex)) {
			throw OtException(OtFormat("Invalid URL [%s]", url.c_str()));
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
				throw OtException(OtFormat("Invalid URL authority [%s]", authority.c_str()));
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
				queryParams[OtTextDecodeURL(std::string((*i)[2].first, (*i)[2].second))] =
					OtTextDecodeURL(std::string((*i)[3].first, (*i)[3].second));
			}
		}

		fragment = submatch(match, 4);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtURLClass>("Path", OtHttpClass::getMeta());
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

	// create a new object
	static OtURL create(const std::string& value) {
		OtURL Path = std::make_shared<OtURLClass>(value);
		Path->setType(getMeta());
		return Path;
	}

private:
	std::string submatch(const std::smatch& match, int index) {
		auto& sub = match[index];
		return std::string(sub.first, sub.second);
	}

	std::string url;
	std::string scheme;
	std::string authority;
	std::string username;
	std::string password;
	std::string host;
	int port {0};
	std::string path;
	std::string directory;
	std::string filename;
	std::string stem;
	std::string extension;
	std::string query;
	std::string fragment;
	std::unordered_map<std::string, std::string> queryParams;
};
