//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <unordered_map>


//
//	OtUrl
//

class OtUrl {
public:
	// constructors
	OtUrl() = default;
	OtUrl(const std::string& urlString) { parse(urlString); }

	// parse URL string
	void parse(const std::string& url);

	// get URL parts
	inline const std::string& getURL() { return url; }
	inline const std::string& getScheme() { return scheme; }
	inline const std::string& getAuthority() { return authority; }
	inline const std::string& getUsername() { return username; }
	inline const std::string& getPassword() { return password; }
	inline const std::string& getHost() { return host; }
	inline int getPort() { return port; }
	inline const std::string& getPath() { return path; }
	inline const std::string& getDirectory() { return directory; }
	inline const std::string& getFilename() { return filename; }
	inline const std::string& getStem() { return stem; }
	inline const std::string& getExtension() { return extension; }
	inline const std::string& getQuery() { return query; }
	inline const std::string& getFragment() { return fragment; }

	inline bool hasParam(const std::string& p) {
		return queryParams.find(p) != queryParams.end();
	}

	std::string getParam(const std::string& p) { return queryParams.find(p)->second; }

	std::unordered_map<std::string, std::string> getParams() {
		return queryParams;
	}

	// access the data
	const std::string& download();
	inline int getStatus() { return status; }
	inline void* getDownloadedData() { return data.data(); }
	inline size_t getDownloadedSize() { return data.size(); }

private:
	// properties
	std::string url;
	std::string scheme;
	std::string authority;
	std::string username;
	std::string password;
	std::string host;
	int port = 80;
	std::string path;
	std::string directory;
	std::string filename;
	std::string stem;
	std::string extension;
	std::string query;
	std::string fragment;
	std::unordered_map<std::string, std::string> queryParams;

	// query results
	int status;
	std::string data;
};
