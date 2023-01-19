//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtBoolean.h"
#include "OtPrimitive.h"
#include "OtInteger.h"
#include "OtReal.h"
#include "OtString.h"


//
//	Overloaded conversion functions for ObjectTalk function calls
//

inline OtObject OtObjectCreate() { return OtObjectClass::create(); }
inline OtObject OtObjectCreate(OtObject value) { return value; }
inline OtObject OtObjectCreate(bool value) { return OtBooleanClass::create(value); }
inline OtObject OtObjectCreate(int value) { return OtIntegerClass::create(value); }
inline OtObject OtObjectCreate(long value) { return OtIntegerClass::create(value); }
inline OtObject OtObjectCreate(size_t value) { return OtIntegerClass::create(value); }
inline OtObject OtObjectCreate(float value) { return OtRealClass::create(value); }
inline OtObject OtObjectCreate(double value) { return OtRealClass::create(value); }
inline OtObject OtObjectCreate(const std::string& value) { return OtStringClass::create(value); }


//
//	OtFunction
//

class OtFunctionClass;
typedef std::shared_ptr<OtFunctionClass> OtFunction;

class OtFunctionClass : public OtPrimitiveClass {
	typedef OtObject (*OtInvoke)(void* target, size_t count, OtObject* parameters);

public:
	// constructors
	OtFunctionClass() = default;
	OtFunctionClass(OtInvoke i, void* t, size_t p) : invoker(i), target(t), parameterCount(p) {}

	// destructor
	~OtFunctionClass() { free(target); }

	// call function
	OtObject operator () (size_t count, OtObject* parameters) override;

	// get parameter count
	size_t getParameterCount() { return parameterCount; }

	// get type definition
	static OtType getMeta();

	// create new function bindings by function type
	static OtFunction create(OtObject (*function)(size_t, OtObject*)) {
		typedef decltype(function) FUNCTION;
		FUNCTION* target = (FUNCTION*) malloc(sizeof(FUNCTION));
		*target = function;

		struct Helper {
			static OtObject invoke(void* t, size_t n, OtObject* p) {
				return (*((FUNCTION*) t))(n, p);
			}
		};

		return createWithType(std::make_shared<OtFunctionClass>(&Helper::invoke, target, SIZE_MAX));
	}

	static OtFunction create(void (*function)(size_t, OtObject*)) {
		typedef decltype(function) FUNCTION;
		FUNCTION* target = (FUNCTION*) malloc(sizeof(FUNCTION));
		*target = function;

		struct Helper {
			static OtObject invoke(void* t, size_t n, OtObject* p) {
				(*((FUNCTION*) t))(n, p);
				return nullptr;
			}
		};

		return createWithType(std::make_shared<OtFunctionClass>(&Helper::invoke, target, SIZE_MAX));
	}

	template<typename RETURN, typename ...ARGS>
	static OtFunction create(RETURN (*function)(ARGS...)) {
		typedef decltype(function) FUNCTION;
		FUNCTION* target = (FUNCTION*) malloc(sizeof(FUNCTION));
		*target = function;

		struct Helper {
			static OtObject invoke(void* t, size_t n, OtObject* p) {
				return invokeFunction<FUNCTION>(t, p, std::index_sequence_for<ARGS...>());
			}
		};

		return createWithType(std::make_shared<OtFunctionClass>(&Helper::invoke, target, sizeof...(ARGS)));
	}

	template<typename ...ARGS>
	static OtFunction create(void (*function)(ARGS...)) {
		typedef decltype(function) FUNCTION;
		FUNCTION* target = (FUNCTION*) malloc(sizeof(FUNCTION));
		*target = function;

		struct Helper {
			static OtObject invoke(void* t, size_t n, OtObject* p) {
				return invokeVoidFunction<FUNCTION>(t, p, std::index_sequence_for<ARGS...>());
			}
		};

		return createWithType(std::make_shared<OtFunctionClass>(&Helper::invoke, target, sizeof...(ARGS)));
	}

	template<typename CLASS>
	static OtFunction create(OtObject (CLASS::*method)(size_t, OtObject*)) {
		typedef decltype(method) METHOD;
		METHOD* target = (METHOD*) malloc(sizeof(METHOD));
		*target = method;

		struct Helper {
			static OtObject invoke(void* t, size_t n, OtObject* p) {
				auto object = p[0]->cast<CLASS>();
				auto method = *((METHOD*) t);
				return ((*object).*method)(n - 1, p + 1);
			}
		};

		return createWithType(std::make_shared<OtFunctionClass>(&Helper::invoke, target, SIZE_MAX));
	}

	template<typename CLASS>
	static OtFunction create(void (CLASS::*method)(size_t, OtObject*)) {
		typedef decltype(method) METHOD;
		METHOD* target = (METHOD*) malloc(sizeof(METHOD));
		*target = method;

		struct Helper {
			static OtObject invoke(void* t, size_t n, OtObject* p) {
				auto object = p[0]->cast<CLASS>();
				auto method = *((METHOD*) t);
				((*object).*method)(n - 1, p + 1);
				return nullptr;
			}
		};

		return createWithType(std::make_shared<OtFunctionClass>(&Helper::invoke, target, SIZE_MAX));
	}

	template<typename CLASS, typename ...ARGS>
	static OtFunction create(void (CLASS::*method)(ARGS...)) {
		typedef decltype(method) METHOD;
		METHOD* target = (METHOD*) malloc(sizeof(METHOD));
		*target = method;

		struct Helper {
			static OtObject invoke(void* t, size_t n, OtObject* p) {
				return invokeVoidMethod<CLASS, METHOD>(t, p, std::index_sequence_for<ARGS...>());
			}
		};

		return createWithType(std::make_shared<OtFunctionClass>(&Helper::invoke, target, sizeof...(ARGS) + 1));
	}

	template<typename CLASS, typename RETURN, typename ...ARGS>
	static OtFunction create(RETURN (CLASS::*method)(ARGS...)) {
		typedef decltype(method) METHOD;
		METHOD* target = (METHOD*) malloc(sizeof(METHOD));
		*target = method;

		struct Helper {
			static OtObject invoke(void* t, size_t n, OtObject* p) {
				return invokeMethod<CLASS, METHOD>(t, p, std::index_sequence_for<ARGS...>());
			}
		};

		return createWithType(std::make_shared<OtFunctionClass>(&Helper::invoke, target, sizeof...(ARGS) + 1));
	}

	template<typename CLASS, typename RETURN, typename ...ARGS>
	static OtFunction create(RETURN (CLASS::*method)(ARGS...) const) {
		typedef decltype(method) METHOD;
		METHOD* target = (METHOD*) malloc(sizeof(METHOD));
		*target = method;

		struct Helper {
			static OtObject invoke(void* t, size_t n, OtObject* p) {
				return invokeMethod<CLASS, METHOD>(t, p, std::index_sequence_for<ARGS...>());
			}
		};

		return createWithType(std::make_shared<OtFunctionClass>(&Helper::invoke, target, sizeof...(ARGS) + 1));
	}

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
		auto object = p[0]->cast<CLASS>();
		auto method = *((METHOD*) t);
		((*object).*method)(*p[I + 1]...);
		return nullptr;
	}

	template<typename CLASS, typename METHOD, size_t... I>
	inline static OtObject invokeMethod(void* t, OtObject* p, std::index_sequence<I...>) {
		auto object = p[0]->cast<CLASS>();
		auto method = *((METHOD*) t);
		return OtObjectCreate(((*object).*method)(*p[I + 1]...));
	}

	// helper function to associate type with object
	static OtFunction createWithType(OtFunction func) {
		func->setType(getMeta());
		return func;
	}

	OtInvoke invoker = nullptr;
	void* target = nullptr;
	size_t parameterCount = 0;
};
