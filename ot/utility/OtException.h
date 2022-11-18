//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	  Include files
//

#include <stdio.h>

#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "OtFormat.h"


//
//	OtException
//

class OtException : public std::exception {
public:
	OtException(
		const std::string& m,
		size_t l,
		size_t s,
		size_t e,
		const std::string& sm,
		const std::string& lm) :
			module(m),
			lineNumber(l),
			start(s),
			end(e),
			shortMessage(sm),
			longMessage(lm) {
	}

	OtException(std::string m) : shortMessage(m), longMessage(m) {}

	const char* what() const throw() { return longMessage.c_str(); }

	// access properties
	std::string getModule() const { return module; }
	size_t getLineNumber() const { return lineNumber; }
	size_t getStart() const { return start; }
	size_t getEnd() const { return end; }
	std::string getShortErrorMessage() const { return shortMessage; }
	std::string getLongErrorMessage() const { return longMessage; }

private:
	// the module in which the error happened
	std::string module;

	// line number causing the error (starting at 1)
	size_t lineNumber;

	// start of token causing the error (in bytes from start of source)
	size_t start;

	// end of token causing the error (in bytes from start of source)
	size_t end;

	// the short and long error message
	std::string shortMessage;
	std::string longMessage;
};


//
//	OtExcept
//

template <typename... ARGS>
void OtExcept(const char* format, ARGS && ...args) {
	if constexpr(sizeof...(ARGS) == 0) {
		throw OtException(format);

	} else {
		auto size = std::snprintf(nullptr, 0, format, std::forward<ARGS>(args)...);
		std::vector<char> buffer(size + 1);
		std::snprintf(buffer.data(), size + 1, format, std::forward<ARGS>(args)...);
		throw OtException(std::string(buffer.data()));
	}
}

inline void OtExcept(const std::string message) {
	throw OtException(message);
}


//
//	Macros
//

#define OT_FATAL(format, ...) OtExcept("%s: line %d: " format, __FILE__, __LINE__, __VA_ARGS__)
#define OT_ASSERT(assertion) if (!(assertion)) OT_FATAL("Assertion error: %s", #assertion)
#define OT_DEBUG(value) std::cout << (value) << std::endl
