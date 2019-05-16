//	ObjectTalk Scripting Language
//	Copyright 1993-2019 Johan A. Goossens
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
//	OtURI
//

class OtURIClass;
typedef std::shared_ptr<OtURIClass> OtURI;


//
//	OtURIClass
//

class OtURIClass : public OtSystemClass
{
public:
	// constructors
	OtURIClass() {}
	OtURIClass(const std::string& path) { parse(path); }

	// initialize URI
	OtObject init(OtObject, size_t count, OtObject* parameters)
	{
		if (count != 1)
			OT_EXCEPT("URI initializer expected 1 parameter not [%d]", count);

		parse(parameters[0]->operator std::string());
		return getSharedPtr();
	}

	// get URI parts
	operator std::string() { return uri; }
	const char* c_str() { return uri.c_str(); }
	const std::string& getURI() { return uri; }
	const std::string& getScheme() { return scheme; }
	const std::string& getAuthority() { return authority; }
	const std::string& getUsername() { return username; }
	const std::string& getPassword() { return password; }
	const std::string& getHost() { return host; }
	long getPort() { return port; }
	const std::string& getPath() { return path; }
	const std::string& getDirectory() { return directory; }
	const std::string& getFilename() { return filename; }
	const std::string& getStem() { return stem; }
	const std::string& getExtension() { return extension; }
	const std::string& getQuery() { return query; }
	const std::string& getFragment() { return fragment; }
	const std::map<std::string, std::string>& getQueryParams() { return queryParams; }

	OtObject getParams()
	{
		OtDict result = OtDictClass::create();

		for (auto& it : getQueryParams())
			result->operator[](it.first) = OtStringClass::create(it.second);

		return result;
	}

	// parse URI string
	void parse(const std::string& uri)
	{
		this->uri = uri;

		static std::regex uriRegex(
			"([a-zA-Z][a-zA-Z0-9+.-]*):"	// scheme:
			"([^?#]*)"						// authority and path
			"(?:\\?([^#]*))?"				// ?query
			"(?:#(.*))?");					// #fragment
		std::smatch match;

		if (!std::regex_match(uri, match, uriRegex))
			OT_EXCEPT("Invalid URI [%s]", uri.c_str());

		scheme = submatch(match, 1);
		std::transform(scheme.begin(), scheme.end(), scheme.begin(), ::tolower);
		path = submatch(match, 2);

		static std::regex authorityAndPathRegex("//([^/]*)(/.*)?");
		std::smatch authorityAndPathMatch;

		if (std::regex_match(path, authorityAndPathMatch, authorityAndPathRegex))
		{
			authority = submatch(authorityAndPathMatch, 1);

			static std::regex authorityRegex(
				"(?:([^@:]*)(?::([^@]*))?@)?"	// username, password
				"(\\[[^\\]]*\\]|[^\\[:]*)"		// host (IP-literal (e.g. '['+IPv6+']', dotted-IPv4, or named host)
				"(?::(\\d*))?");				// port

			std::smatch authorityMatch;

			if (!std::regex_match(authority, authorityMatch, authorityRegex))
				OT_EXCEPT("Invalid URI authority [%s]", authority.c_str());

			username = submatch(authorityMatch, 1);
			password = submatch(authorityMatch, 2);
			host = submatch(authorityMatch, 3);
			auto portString = submatch(authorityMatch, 4);
			port = (portString.size()) ? std::stoi(portString) : 0;

			path = submatch(authorityAndPathMatch, 2);

			auto slash = path.rfind('/');

			if (slash == std::string::npos)
			{
				directory = "";
				filename = path;
			}

			else
			{
				directory = path.substr(0, slash);
				filename = path.substr(slash + 1);
			}

			auto period = filename.rfind('.');

			if (period == std::string::npos)
			{
				stem = filename;
				extension = "";
			}

			else
			{
				stem = filename.substr(0, period);
				extension = filename.substr(period + 1);
				std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
			}
		}

		query = submatch(match, 3);

		if (query.size())
		{
			static std::regex queryParamRegex(
				"(^|&)"			// start of query or start of parameter "&"
				"([^=&]*)=?"	// parameter name and "=" if value is expected
				"([^=&]*)"		// parameter value
				"(?=(&|$))");	// next should be end of query or start of next parameter

			for (auto i = std::sregex_iterator(query.begin(), query.end(), queryParamRegex); i != std::sregex_iterator(); i++)
				queryParams[std::string((*i)[2].first, (*i)[2].second)] = std::string((*i)[3].first, (*i)[3].second);
		}

		fragment = submatch(match, 4);
	}

	// helper class to track libcurl handle
	class OtCurl
	{
	public:
		OtCurl() { curl_global_init(CURL_GLOBAL_ALL); curl = curl_easy_init(); }
		~OtCurl() { curl_easy_cleanup(curl); curl_global_cleanup(); }

		static CURL* instance()
		{
			static std::shared_ptr<OtCurl> singleton;

			if (!singleton)
				singleton = std::make_shared<OtCurl>();

			else
				curl_easy_reset(singleton->curl);

			return singleton->curl;
		}

	private:
		CURL* curl;
	};

	// helper function to read data from stream
	static size_t readData(char* pointer, size_t size, size_t count, void* stream)
	{
		return ((std::istream*) stream)->readsome(pointer, size * count);
	}

	// helper function to write data to stream
	static size_t writeData(char* pointer, size_t size, size_t count, void* stream)
	{
		((std::ostream*) stream)->write(pointer, size * count);
		return size * count;
	}

	// helper function to trim string
	static std::string trim(const std::string& value)
	{
		const auto begin = value.find_first_not_of(" \t\n\r\f\v");

		if (begin == std::string::npos)
			return "";

		else
			return value.substr(begin, value.find_last_not_of(" \t\n\r\f\v") - begin + 1);
	}

	// get URI as string
	OtObject head()
	{
		std::stringstream header;

		CURL* curl = OtCurl::instance();
		curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &OtURIClass::writeData);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header);
		CURLcode res = curl_easy_perform(curl);

		if (res != CURLE_OK)
			OT_EXCEPT("Error [%s]", curl_easy_strerror(res));

		OtDict headers = OtDictClass::create();
		std::string line;

		std::getline(header, line);
		headers->operator[] ("Result") = OtStringClass::create(trim(line));

		while (std::getline(header, line) && line != "\r")
		{
			size_t index = line.find(':', 0);

			if (index != std::string::npos)
				headers->operator[] (trim(line.substr(0, index))) =
					OtStringClass::create(trim(line.substr(index + 1)));
		}

		return headers;
	}

	// get URI as string
	std::string getString()
	{
		std::ostringstream out;

		CURL* curl = OtCurl::instance();
		curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &OtURIClass::writeData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
		CURLcode res = curl_easy_perform(curl);

		if (res != CURLE_OK)
			OT_EXCEPT("Error [%s]", curl_easy_strerror(res));

		return out.str();
	}

	// get URI as string
	void getFile(const std::string& filename)
	{
		std::ofstream out;
		out.open(filename.c_str());

		CURL* curl = OtCurl::instance();
		curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &OtURIClass::writeData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
		CURLcode res = curl_easy_perform(curl);
		out.close();

		if (res != CURLE_OK)
			OT_EXCEPT("Error [%s]", curl_easy_strerror(res));
	}

	// put string to URI
	void putString(const std::string& string)
	{
		std::istringstream in(string);

		CURL* curl = OtCurl::instance();
		curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
		curl_easy_setopt(curl, CURLOPT_PUT, 1);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, &OtURIClass::readData);
		curl_easy_setopt(curl, CURLOPT_READDATA, &in);
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, string.length());
		CURLcode res = curl_easy_perform(curl);

		if (res != CURLE_OK)
			OT_EXCEPT("Error [%s]", curl_easy_strerror(res));
	}

	// put file to URI
	void putFile(const std::string& filename)
	{
		std::ifstream in;
		in.open(filename.c_str());

		CURL* curl = OtCurl::instance();
		curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
		curl_easy_setopt(curl, CURLOPT_PUT, 1);
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, &OtURIClass::readData);
		curl_easy_setopt(curl, CURLOPT_READDATA, &in);
		CURLcode res = curl_easy_perform(curl);
		in.close();

		if (res != CURLE_OK)
			OT_EXCEPT("Error [%s]", curl_easy_strerror(res));
	}

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtURIClass>("Path", OtSystemClass::getMeta());

			type->set("__init__", OtFunctionClass::create(&OtURIClass::init));

			type->set("scheme", OtFunctionCreate(&OtURIClass::getScheme));
			type->set("authority", OtFunctionCreate(&OtURIClass::getAuthority));
			type->set("user", OtFunctionCreate(&OtURIClass::getUsername));
			type->set("password", OtFunctionCreate(&OtURIClass::getPassword));
			type->set("host", OtFunctionCreate(&OtURIClass::getHost));
			type->set("port", OtFunctionCreate(&OtURIClass::getPort));
			type->set("path", OtFunctionCreate(&OtURIClass::getPath));
			type->set("directory", OtFunctionCreate(&OtURIClass::getDirectory));
			type->set("filename", OtFunctionCreate(&OtURIClass::getFilename));
			type->set("stem", OtFunctionCreate(&OtURIClass::getStem));
			type->set("extension", OtFunctionCreate(&OtURIClass::getExtension));
			type->set("query", OtFunctionCreate(&OtURIClass::getQuery));
			type->set("params", OtFunctionCreate(&OtURIClass::getParams));
			type->set("fragment", OtFunctionCreate(&OtURIClass::getFragment));

			type->set("head", OtFunctionCreate(&OtURIClass::head));
			type->set("getString", OtFunctionCreate(&OtURIClass::getString));
			type->set("getFile", OtFunctionCreate(&OtURIClass::getFile));
			type->set("putString", OtFunctionCreate(&OtURIClass::putString));
			type->set("putFile", OtFunctionCreate(&OtURIClass::putFile));
		}

		return type;
	}

	// create a new object
	static OtURI create(const std::string& value)
	{
		OtURI Path = std::make_shared<OtURIClass>(value);
		Path->setType(getMeta());
		return Path;
	}

private:
	std::string submatch(const std::smatch& match, int index)
	{
		auto& sub = match[index];
		return std::string(sub.first, sub.second);
	}

	std::string uri;
	std::string scheme;
	std::string authority;
	std::string username;
	std::string password;
	std::string host;
	int port;
	std::string path;
	std::string directory;
	std::string filename;
	std::string stem;
	std::string extension;
	std::string query;
	std::string fragment;
	std::map<std::string, std::string> queryParams;
};
