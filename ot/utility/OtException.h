//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <exception>
#include <string>

#include "fmt/format.h"


//
//	OtException
//

class OtException : public std::exception {
public:
	// constructors
	OtException() = default;

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

	inline const char* what() const noexcept { return longMessage.c_str(); }

	// access properties
	inline std::string getModule() const { return module; }
	inline size_t getLineNumber() const { return lineNumber; }
	inline size_t getStart() const { return start; }
	inline size_t getEnd() const { return end; }
	inline std::string getShortErrorMessage() const { return shortMessage; }
	inline std::string getLongErrorMessage() const { return longMessage; }

	// (de)serializer
	std::string serialize();
	void deserialize(const std::string& string);

private:
	// the module in which the error happened
	std::string module;

	// line number causing the error (starting at 1)
	size_t lineNumber = 0;

	// start of token causing the error (in bytes from start of source)
	size_t start = 0;

	// end of token causing the error (in bytes from start of source)
	size_t end = 0;

	// the short and long error message
	std::string shortMessage;
	std::string longMessage;
};


//
//	OtError
//

template <typename... ARGS>
inline void OtError(const char* format, ARGS&& ...args) {
	if constexpr(sizeof...(ARGS) == 0) {
		throw OtException(format);

	} else {
		auto message = fmt::format(format, args...);
		throw OtException(message);
	}
}

inline void OtError(const std::string message) {
	throw OtException(message);
}
