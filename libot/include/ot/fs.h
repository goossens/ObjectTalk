//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "system.h"


//
//	OtFS
//

class OtFSClass;
typedef std::shared_ptr<OtFSClass> OtFS;

class OtFSClass : public OtSystemClass {
public:
	// get temporary directory
	OtObject gettmp();

	// get temporary file name
	OtObject tmpnam();

	// get current working directory
	OtObject getcwd();

	// change current working directory
	void chdir(const std::string& path);

	// get list of files in specified directory as an array
	OtObject ls(const std::string& path);

	// get file size
	size_t filesize(const std::string& path);

	// touch a file
	void touch(const std::string& path);

	// copy a file system object
	void cp(const std::string& from, const std::string& to);

	// move (rename) a file system object
	void mv(const std::string& from, const std::string& to);

	// link a file system object
	void ln(const std::string& from, const std::string& to);

	// link a file system object symbolically
	void lns(const std::string& from, const std::string& to);

	// resize file
	void resize(const std::string& path, size_t size);

	// remove file system object
	void rm(const std::string& path);

	// create directory
	void mkdir(const std::string& path);

	// create (intermediate) directories
	void mkdirs(const std::string& path);

	// create temporary directory
	OtObject mktmpdir();

	// remove directory
	void rmdir(const std::string& path);

	// recursively remove directory and its content
	void rmdirs(const std::string& path);

	// get file system capacity
	size_t capacity(const std::string& path);

	// get free space on file system
	size_t free(const std::string& path);

	// get available space on file system
	size_t available(const std::string& path);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtFS create();
};
