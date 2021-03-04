//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtException
//

class OtException {
public:
	OtException(std::string m) : message(m) {}
	virtual const char* what() const throw() { return message.c_str(); }

private:
	std::string message;
};

#define OT_EXCEPT(format, ...) throw OtException(OtFormat("%s: line %d: " format, __FILE__, __LINE__, __VA_ARGS__))
#define OT_ASSERT(assertion) if (!assertion) OT_EXCEPT("Assertion error: %s", #assertion)
#define OT_DEBUG(value) std::cout << value << std::endl
