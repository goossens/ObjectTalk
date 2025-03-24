//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <unordered_map>

#include "OtHttp.h"
#include "OtString.h"


//
//	OtURL
//

class OtURLClass;
using OtURL = OtObjectPointer<OtURLClass>;

class OtURLClass : public OtHttpClass
{
public:
	// initialize URL
	void init(size_t count, OtObject* parameters);

	// get URL parts
	operator std::string() { return url; }
	const std::string& getURL() { return url; }
	const std::string& getScheme() { return scheme; }
	const std::string& getAuthority() { return authority; }
	const std::string& getUsername() { return username; }
	const std::string& getPassword() { return password; }
	const std::string& getHost() { return host; }
	int getPort() { return port; }
	const std::string& getPath() { return path; }
	const std::string& getDirectory() { return directory; }
	const std::string& getFilename() { return filename; }
	const std::string& getStem() { return stem; }
	const std::string& getExtension() { return extension; }
	const std::string& getQuery() { return query; }
	const std::string& getFragment() { return fragment; }

	bool hasParam(const std::string& p) {
		return queryParams.find(p) != queryParams.end();
	}

	OtObject getParam(const std::string& p) {
		return OtString::create(queryParams.find(p)->second);
	}

	OtObject getParamWithDefault(const std::string& p, const OtObject d) {
		return hasParam(p) ? getParam(p) : d;
	}

	std::unordered_map<std::string, std::string> getParams() {
		return queryParams;
	}

	// parse URL string
	void parse(const std::string& url);

	// get type definition
	static OtType getMeta();

protected:
	// constructors
	friend class OtObjectPointer<OtURLClass>;
	OtURLClass() = default;
	OtURLClass(const std::string& path) { parse(path); }

private:
	// properties
	std::string url;
	std::string scheme;
	std::string authority;
	std::string username;
	std::string password;
	std::string host;
	int port = 0;
	std::string path;
	std::string directory;
	std::string filename;
	std::string stem;
	std::string extension;
	std::string query;
	std::string fragment;
	std::unordered_map<std::string, std::string> queryParams;
};
