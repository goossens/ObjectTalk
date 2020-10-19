//	ObjectTalk Scripting Language
//	Copyright 1993-2020 Johan A. Goossens
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
//	OtURL
//

class OtURLClass;
typedef std::shared_ptr<OtURLClass> OtURL;


//
//	OtURLClass
//

class OtURLClass : public OtNetClass
{
public:
	// constructors
	OtURLClass() = default;
	OtURLClass(const std::wstring& path) { parse(path); }

	// initialize URL
	OtObject init(OtObject, size_t count, OtObject* parameters) {
		if (count != 1) {
			OT_EXCEPT(L"URL initializer expected 1 parameter not [%d]", count);
		}

		parse(parameters[0]->operator std::wstring());
		return getSharedPtr();
	}

	// get URL parts
	operator std::wstring() { return url; }
	const std::wstring& getURL() { return url; }
	const std::wstring& getScheme() { return scheme; }
	const std::wstring& getAuthority() { return authority; }
	const std::wstring& getUsername() { return username; }
	const std::wstring& getPassword() { return password; }
	const std::wstring& getHost() { return host; }
	const long getPort() { return port; }
	const std::wstring& getPath() { return path; }
	const std::wstring& getDirectory() { return directory; }
	const std::wstring& getFilename() { return filename; }
	const std::wstring& getStem() { return stem; }
	const std::wstring& getExtension() { return extension; }
	const std::wstring& getQuery() { return query; }
	const std::wstring& getFragment() { return fragment; }

	const bool hasParam(const std::wstring& p) {
		return queryParams.find(p) != queryParams.end();
 	}

	const OtObject getParam(const std::wstring& p) {
		return OtStringClass::create(queryParams.find(p)->second);
	}

	const OtObject getParamWithDefault(const std::wstring& p, const OtObject d) {
		return hasParam(p) ? getParam(p) : d;
	}

	// parse URL string
	void parse(const std::wstring& url) {
		this->url = url;

		static std::wregex urlRegex(
			L"([a-zA-Z][a-zA-Z0-9+.-]*):"	// scheme:
			L"([^?#]*)"						// authority and path
			L"(?:\\?([^#]*))?"				// ?query
			L"(?:#(.*))?");					// #fragment

		std::wsmatch match;

		if (!std::regex_match(url, match, urlRegex)) {
			OT_EXCEPT(L"Invalid URL [%s]", url.c_str());
		}

		scheme = submatch(match, 1);
		std::transform(scheme.begin(), scheme.end(), scheme.begin(), ::tolower);
		path = submatch(match, 2);

		static std::wregex authorityAndPathRegex(L"//([^/]*)(/.*)?");
		std::wsmatch authorityAndPathMatch;

		if (std::regex_match(path, authorityAndPathMatch, authorityAndPathRegex)) {
			authority = submatch(authorityAndPathMatch, 1);

			static std::wregex authorityRegex(
				L"(?:([^@:]*)(?::([^@]*))?@)?"	// username, password
				L"(\\[[^\\]]*\\]|[^\\[:]*)"		// host (IP-literal (e.g. '['+IPv6+']', dotted-IPv4, or named host)
				L"(?::(\\d*))?");				// port

			std::wsmatch authorityMatch;

			if (!std::regex_match(authority, authorityMatch, authorityRegex)) {
				OT_EXCEPT(L"Invalid URL authority [%s]", authority.c_str());
			}

			username = submatch(authorityMatch, 1);
			password = submatch(authorityMatch, 2);
			host = submatch(authorityMatch, 3);
			auto portString = submatch(authorityMatch, 4);
			port = (portString.size()) ? std::stoi(portString) : 0;

			path = submatch(authorityAndPathMatch, 2);

			auto slash = path.rfind('/');

			if (slash == std::wstring::npos) {
				directory = L"";
				filename = path;

			} else {
				directory = path.substr(0, slash);
				filename = path.substr(slash + 1);
			}

			auto period = filename.rfind('.');

			if (period == std::wstring::npos) {
				stem = filename;
				extension = L"";

			} else {
				stem = filename.substr(0, period);
				extension = filename.substr(period + 1);
				std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
			}
		}

		query = submatch(match, 3);

		if (query.size()) {
			static std::wregex queryParamRegex(
				L"(^|&)"		// start of query or start of parameter "&"
				L"([^=&]*)=?"	// parameter name and "=" if value is expected
				L"([^=&]*)"		// parameter value
				L"(?=(&|$))");	// next should be end of query or start of next parameter

			for (auto i = std::wsregex_iterator(query.begin(), query.end(), queryParamRegex); i != std::wsregex_iterator(); i++) {
				queryParams[std::wstring((*i)[2].first, (*i)[2].second)] = std::wstring((*i)[3].first, (*i)[3].second);
			}
		}

		fragment = submatch(match, 4);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtURLClass>(L"Path", OtNetClass::getMeta());
			type->set(L"__init__", OtFunctionClass::create(&OtURLClass::init));

			type->set(L"url", OtFunctionCreate(&OtURLClass::getURL));
			type->set(L"scheme", OtFunctionCreate(&OtURLClass::getScheme));
			type->set(L"authority", OtFunctionCreate(&OtURLClass::getAuthority));
			type->set(L"user", OtFunctionCreate(&OtURLClass::getUsername));
			type->set(L"password", OtFunctionCreate(&OtURLClass::getPassword));
			type->set(L"host", OtFunctionCreate(&OtURLClass::getHost));
			type->set(L"port", OtFunctionCreate(&OtURLClass::getPort));
			type->set(L"path", OtFunctionCreate(&OtURLClass::getPath));
			type->set(L"directory", OtFunctionCreate(&OtURLClass::getDirectory));
			type->set(L"filename", OtFunctionCreate(&OtURLClass::getFilename));
			type->set(L"stem", OtFunctionCreate(&OtURLClass::getStem));
			type->set(L"extension", OtFunctionCreate(&OtURLClass::getExtension));
			type->set(L"query", OtFunctionCreate(&OtURLClass::getQuery));
			type->set(L"fragment", OtFunctionCreate(&OtURLClass::getFragment));

			type->set(L"hasParam", OtFunctionCreate(&OtURLClass::hasParam));
			type->set(L"getParam", OtFunctionCreate(&OtURLClass::getParam));
			type->set(L"getParamWithDefault", OtFunctionCreate(&OtURLClass::getParamWithDefault));
		}

		return type;
	}

	// create a new object
	static OtURL create(const std::wstring& value) {
		OtURL Path = std::make_shared<OtURLClass>(value);
		Path->setType(getMeta());
		return Path;
	}

private:
	std::wstring submatch(const std::wsmatch& match, int index) {
		auto& sub = match[index];
		return std::wstring(sub.first, sub.second);
	}

	std::wstring url;
	std::wstring scheme;
	std::wstring authority;
	std::wstring username;
	std::wstring password;
	std::wstring host;
	int port {0};
	std::wstring path;
	std::wstring directory;
	std::wstring filename;
	std::wstring stem;
	std::wstring extension;
	std::wstring query;
	std::wstring fragment;
	std::map<std::wstring, std::wstring> queryParams;
};
