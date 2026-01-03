//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <string>


//
//	OtPrimitive
//

class OtSourceClass;
typedef std::shared_ptr<OtSourceClass> OtSource;

class OtSourceClass {
public:
	// constructor
	OtSourceClass(const std::string& m, const std::string& s) : module(m), source(s) {}

	// return size of source code
	inline size_t size() { return source.size(); }

	// access source code at position
	inline char at(size_t index) { return source[index]; }

	// get part of source code
	std::string substr(size_t position, size_t size) { return source.substr(position, size); }

	// get start/end of line
	size_t getStartOfLine(size_t location);
	size_t getEndOfLine(size_t location);

	// get line of source code that includes specified location
	std::string getLine(size_t location);

	// get lines of source code that includes specified locations
	std::string getLines(size_t start, size_t end);

	// get line number of specified location
	size_t getLineNumber(size_t location);

	// get module
	std::string getModule() { return module; }

	// create a new instance
	static OtSource create(const std::string& module, const std::string& source);

private:
	std::string module;
	std::string source;
};
