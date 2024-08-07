//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <tuple>
#include <utility>

#include "OtPrimitive.h"
#include "OtValue.h"
#include "OtVM.h"


//
//	OtFunction
//

class OtFunctionClass;
using OtFunction = OtObjectPointer<OtFunctionClass>;

class OtFunctionClass : public OtPrimitiveClass {
public:
	// constructors
	OtFunctionClass() = default;

	OtFunctionClass(void (*function)(size_t, OtObject*)) {
		caller = [function](size_t count, OtObject* parameters) {
			function(count, parameters);
			return OtVM::instance()->getNull();
		};

		parameterCount = SIZE_MAX;
	}

	OtFunctionClass(OtObject (*function)(size_t, OtObject*)) {
		caller = [function](size_t count, OtObject* parameters) {
			return function(count, parameters);
		};

		parameterCount = SIZE_MAX;
	}

	template <typename Args, std::size_t... I>
	static auto functionArgs(OtObject* parameters, std::index_sequence<I...>) {
		return std::make_tuple(OtValue<std::tuple_element_t<I, Args>>::decode(parameters[I])...);
	}

	template<typename ...Args>
	OtFunctionClass(void (*function)(Args...)) {
		caller = [function](size_t count, OtObject* parameters) {
			std::apply(
				function,
				functionArgs<typename std::tuple<Args...>>(
					parameters,
					std::make_index_sequence<sizeof...(Args)>()));

			return OtVM::instance()->getNull();
		};

		parameterCount = sizeof...(Args);
	}

	template<typename Result, typename ...Args>
	OtFunctionClass(Result (*function)(Args...)) {
		caller = [function](size_t count, OtObject* parameters) {
			return OtValue<Result>::encode(
				std::apply(
					function,
					functionArgs<typename std::tuple<Args...>>(
						parameters,
						std::make_index_sequence<sizeof...(Args)>())));
		};

		parameterCount = sizeof...(Args);
	}

	template<typename ...Args>
	OtFunctionClass(std::function<void(Args...)> function) {
		caller = [function](size_t count, OtObject* parameters) {
			std::apply(function,
				functionArgs<typename std::tuple<Args...>>(
					parameters,
					std::make_index_sequence<sizeof...(Args)>()));

			return OtVM::instance()->getNull();
		};

		parameterCount = sizeof...(Args);
	}

	template<typename Result, typename ...Args>
	OtFunctionClass(std::function<Result(Args...)> function) {
		caller = [function](size_t count, OtObject* parameters) {
			return OtValue<Result>::encode(
				std::apply(
					function,
					functionArgs<typename std::tuple<Args...>>(
						parameters,
						std::make_index_sequence<sizeof...(Args)>())));
		};

		parameterCount = sizeof...(Args);
	}

	template<typename Class>
	OtFunctionClass(void (Class::*method)(size_t, OtObject*)) {
		caller = [method](size_t count, OtObject* parameters) {
			OtObjectPointer<Class> object = parameters[0];
			((*object).*method)(count - 1, parameters + 1);
			return OtVM::instance()->getNull();
		};

		parameterCount = SIZE_MAX;
	}

	template<typename Class>
	OtFunctionClass(OtObject (Class::*method)(size_t, OtObject*)) {
		caller = [method](size_t count, OtObject* parameters) {
			OtObjectPointer<Class> object = parameters[0];
			return ((*object).*method)(count - 1, parameters + 1);
		};

		parameterCount = SIZE_MAX;
	}

	template <typename Class, typename Args, std::size_t... I>
	static auto methodArgs(OtObject* parameters, std::index_sequence<I...>) {
		return std::make_tuple(
			OtObjectPointer<Class>(parameters[0]),
			OtValue<std::tuple_element_t<I, Args>>::decode(parameters[I + 1])...);
	}

	template<typename Class, typename ...Args>
	OtFunctionClass(void (Class::*method)(Args...)) {
		caller = [method](size_t count, OtObject* parameters) {
			std::apply(
				method,
				methodArgs<Class, typename std::tuple<Args...>>(
					parameters,
					std::make_index_sequence<sizeof...(Args)>()));

			return OtVM::instance()->getNull();
		};

		parameterCount = sizeof...(Args) + 1;
	}

	template<typename Class, typename Result, typename ...Args>
	OtFunctionClass(Result (Class::*method)(Args...)) {
		caller = [method](size_t count, OtObject* parameters) {
			return OtValue<Result>::encode(
				std::apply(
					method,
					methodArgs<Class, typename std::tuple<Args...>>(
						parameters,
						std::make_index_sequence<sizeof...(Args)>())));
		};

		parameterCount = sizeof...(Args) + 1;
	}

	template<typename Class, typename Result, typename ...Args>
	OtFunctionClass(Result (Class::*method)(Args...) const) {
		caller = [method](size_t count, OtObject* parameters) {
			return OtValue<Result>::encode(
				std::apply(
					method,
					methodArgs<Class, typename std::tuple<Args...>>(
						parameters,
						std::make_index_sequence<sizeof...(Args)>())));
		};

		parameterCount = sizeof...(Args) + 1;
	}

	// call function
	OtObject operator()(size_t count, OtObject* parameters) override;

	// get parameter count
	size_t getParameterCount() { return parameterCount; }

	// get type definition
	static OtType getMeta();

protected:
	std::function<OtObject(size_t, OtObject*)> caller;
	size_t parameterCount = 0;
};
