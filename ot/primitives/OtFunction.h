//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>

#include "OtBoolean.h"
#include "OtPrimitive.h"
#include "OtInteger.h"
#include "OtReal.h"
#include "OtString.h"


//
//	Overloaded conversion functions for ObjectTalk function calls
//

inline OtObject OtObjectCreate() { return OtObject::create(); }
inline OtObject OtObjectCreate(OtObject value) { return value; }
inline OtObject OtObjectCreate(bool value) { return OtBoolean::create(value); }
inline OtObject OtObjectCreate(int value) { return OtInteger::create(value); }
inline OtObject OtObjectCreate(int64_t value) { return OtInteger::create(value); }
inline OtObject OtObjectCreate(size_t value) { return OtInteger::create(value); }
inline OtObject OtObjectCreate(float value) { return OtReal::create(value); }
inline OtObject OtObjectCreate(double value) { return OtReal::create(value); }
inline OtObject OtObjectCreate(const std::string& value) { return OtString::create(value); }
inline OtObject OtObjectCreate(const std::filesystem::path& value) { return OtString::create(value.string()); }


//
//	OtFunction
//

class OtFunctionClass;
using OtFunction = OtObjectPointer<OtFunctionClass>;

class OtFunctionClass : public OtPrimitiveClass {
	typedef OtObject (*OtInvoke)(void* target, size_t count, OtObject* parameters);

public:
	// constructors
	OtFunctionClass() = default;

	OtFunctionClass(OtObject (*function)(size_t, OtObject*)) {
		typedef decltype(function) FUNCTION;

		struct Helper {
			static OtObject invoke(void* t, size_t n, OtObject* p) {
				return (*((FUNCTION*) t))(n, p);
			}
		};

		invoker = &Helper::invoke;
		target = malloc(sizeof(FUNCTION));
		*((FUNCTION*) target) = function;
		parameterCount = SIZE_MAX;
	}

	OtFunctionClass(void (*function)(size_t, OtObject*)) {
		typedef decltype(function) FUNCTION;

		struct Helper {
			static OtObject invoke(void* t, size_t n, OtObject* p) {
				(*((FUNCTION*) t))(n, p);
				return nullptr;
			}
		};

		invoker = &Helper::invoke;
		target = malloc(sizeof(FUNCTION));
		*((FUNCTION*) target) = function;
		parameterCount = SIZE_MAX;
	}

	template<typename RETURN, typename ...ARGS>
	OtFunctionClass(RETURN (*function)(ARGS...)) {
		typedef decltype(function) FUNCTION;

		struct Helper {
			static OtObject invoke(void* t, size_t n, OtObject* p) {
				return invokeFunction<FUNCTION>(t, p, std::index_sequence_for<ARGS...>());
			}
		};

		invoker = &Helper::invoke;
		target = malloc(sizeof(FUNCTION));
		*((FUNCTION*) target) = function;
		parameterCount = sizeof...(ARGS);
	}

	template<typename ...ARGS>
	OtFunctionClass(void (*function)(ARGS...)) {
		typedef decltype(function) FUNCTION;

		struct Helper {
			static OtObject invoke(void* t, size_t n, OtObject* p) {
				return invokeVoidFunction<FUNCTION>(t, p, std::index_sequence_for<ARGS...>());
			}
		};

		invoker = &Helper::invoke;
		target = malloc(sizeof(FUNCTION));
		*((FUNCTION*) target) = function;
		parameterCount = sizeof...(ARGS);
	}

	template<typename CLASS>
	OtFunctionClass(OtObject (CLASS::*method)(size_t, OtObject*)) {
		typedef decltype(method) METHOD;

		struct Helper {
			static OtObject invoke(void* t, size_t n, OtObject* p) {
				OtObjectPointer<CLASS> object = p[0];
				auto method = *((METHOD*) t);
				return ((*object).*method)(n - 1, p + 1);
			}
		};

		invoker = &Helper::invoke;
		target = malloc(sizeof(METHOD));
		*((METHOD*) target) = method;
		parameterCount = SIZE_MAX;
	}

	template<typename CLASS>
	OtFunctionClass(void (CLASS::*method)(size_t, OtObject*)) {
		typedef decltype(method) METHOD;

		struct Helper {
			static OtObject invoke(void* t, size_t n, OtObject* p) {
				OtObjectPointer<CLASS> object = p[0];
				auto method = *((METHOD*) t);
				((*object).*method)(n - 1, p + 1);
				return nullptr;
			}
		};

		invoker = &Helper::invoke;
		target = malloc(sizeof(METHOD));
		*((METHOD*) target) = method;
		parameterCount = SIZE_MAX;
	}

	template<typename CLASS, typename ...ARGS>
	OtFunctionClass(void (CLASS::*method)(ARGS...)) {
		typedef decltype(method) METHOD;

		struct Helper {
			static OtObject invoke(void* t, size_t n, OtObject* p) {
				return invokeVoidMethod<CLASS, METHOD>(t, p, std::index_sequence_for<ARGS...>());
			}
		};

		invoker = &Helper::invoke;
		target = malloc(sizeof(METHOD));
		*((METHOD*) target) = method;
		parameterCount = sizeof...(ARGS) + 1;
	}

	template<typename CLASS, typename RETURN, typename ...ARGS>
	OtFunctionClass(RETURN (CLASS::*method)(ARGS...)) {
		typedef decltype(method) METHOD;

		struct Helper {
			static OtObject invoke(void* t, size_t n, OtObject* p) {
				return invokeMethod<CLASS, METHOD>(t, p, std::index_sequence_for<ARGS...>());
			}
		};

		invoker = &Helper::invoke;
		target = malloc(sizeof(METHOD));
		*((METHOD*) target) = method;
		parameterCount = sizeof...(ARGS) + 1;
	}

	template<typename CLASS, typename RETURN, typename ...ARGS>
	OtFunctionClass(RETURN (CLASS::*method)(ARGS...) const) {
		typedef decltype(method) METHOD;

		struct Helper {
			static OtObject invoke(void* t, size_t n, OtObject* p) {
				return invokeMethod<CLASS, METHOD>(t, p, std::index_sequence_for<ARGS...>());
			}
		};

		invoker = &Helper::invoke;
		target = malloc(sizeof(METHOD));
		*((METHOD*) target) = method;
		parameterCount = sizeof...(ARGS) + 1;
	}

	// destructor
	~OtFunctionClass() { free(target); }

	// call function
	OtObject operator()(size_t count, OtObject* parameters) override;

	// get parameter count
	size_t getParameterCount() { return parameterCount; }

	// get type definition
	static OtType getMeta();

protected:
	// invoke functions by type
	template<typename FUNCTION, size_t... I>
	inline static OtObject invokeVoidFunction(void* t, OtObject* p, std::index_sequence<I...>) {
		auto func = *((FUNCTION*) t);
		func(*p[I]...);
		return nullptr;
	}

	template<typename FUNCTION, size_t... I>
	inline static OtObject invokeFunction(void* t, OtObject* p, std::index_sequence<I...>) {
		auto func = *((FUNCTION*) t);
		return OtObjectCreate(func(*p[I]...));
	}

	template<typename CLASS, typename METHOD, size_t... I>
	inline static OtObject invokeVoidMethod(void* t, OtObject* p, std::index_sequence<I...>) {
		OtObjectPointer<CLASS> object = p[0];
		auto method = *((METHOD*) t);
		((*object).*method)(*p[I + 1]...);
		return nullptr;
	}

	template<typename CLASS, typename METHOD, size_t... I>
	inline static OtObject invokeMethod(void* t, OtObject* p, std::index_sequence<I...>) {
		OtObjectPointer<CLASS> object = p[0];
		auto method = *((METHOD*) t);
		return OtObjectCreate(((*object).*method)(*p[I + 1]...));
	}

	OtInvoke invoker = nullptr;
	void* target = nullptr;
	size_t parameterCount = 0;
};
