//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtBoolean.h"
#include "OtInteger.h"
#include "OtObject.h"
#include "OtReal.h"
#include "OtString.h"


//
//	OtValue
//
//	Cast C++ types to Objects and visa versa
//

template <class T>
struct OtValue;

template <>
struct OtValue<void> {
	static inline OtObject encode() { return nullptr; }
};

template <>
struct OtValue<bool> {
	static inline OtObject encode(bool value) { return OtBoolean::create(value); }
	static inline bool decode(OtObject object) { return object->operator bool(); }
};

template <>
struct OtValue<int> {
	static inline OtObject encode(int value) { return OtInteger::create(value); }
	static inline int decode(OtObject object) { return object->operator int(); }
};

template <>
struct OtValue<int64_t> {
	static inline OtObject encode(int64_t value) { return OtInteger::create(value); }
	static inline int64_t decode(OtObject object) { return object->operator int64_t(); }
};

template <>
struct OtValue<size_t> {
	static inline OtObject encode(size_t value) { return OtInteger::create(value); }
	static inline size_t decode(OtObject object) { return object->operator size_t(); }
};

template <>
struct OtValue<float> {
	static inline OtObject encode(float value) { return OtReal::create(value); }
	static inline float decode(OtObject object) { return object->operator float(); }
};

template <>
struct OtValue<double> {
	static inline OtObject encode(double value) { return OtReal::create(value); }
	static inline double decode(OtObject object) { return object->operator double(); }
};

template <>
struct OtValue<std::string> {
	static inline OtObject encode(const std::string& value) { return OtString::create(value); }
	static inline std::string decode(OtObject object) { return object->operator std::string(); }
};

template <>
struct OtValue<const std::string&> {
	static inline OtObject encode(const std::string& value) { return OtString::create(value); }
	static inline std::string decode(OtObject object) { return object->operator std::string(); }
};

template <>
struct OtValue<OtObject> {
	static inline OtObject encode(OtObject value) { return value; }
	static inline OtObject decode(OtObject object) { return object; }
};
