//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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


//
//	OtFunction
//

class OtFunctionClass;
using OtFunction = OtObjectPointer<OtFunctionClass>;

class OtFunctionClass : public OtPrimitiveClass {
public:
	template <typename Args, std::size_t... I>
	static inline auto functionArgs(OtObject* parameters, std::index_sequence<I...>) {
		return std::make_tuple(OtValue<std::tuple_element_t<I, Args>>::decode(parameters[I])...);
	}

	template<typename ...Args>
	inline OtFunctionClass(void (*function)(Args...)) {
		caller = [function]([[maybe_unused]] size_t count, OtObject* parameters) {
			std::apply(
				function,
				functionArgs<typename std::tuple<Args...>>(
					parameters,
					std::make_index_sequence<sizeof...(Args)>()));

			return nullptr;
		};

		parameterCount = sizeof...(Args);
	}

	template<typename Result, typename ...Args>
	inline OtFunctionClass(Result (*function)(Args...)) {
		caller = [function]([[maybe_unused]] size_t count, OtObject* parameters) {
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
	inline OtFunctionClass(std::function<void(Args...)> function) {
		caller = [function]([[maybe_unused]] size_t count, OtObject* parameters) {
			std::apply(function,
				functionArgs<typename std::tuple<Args...>>(
					parameters,
					std::make_index_sequence<sizeof...(Args)>()));

			return nullptr;
		};

		parameterCount = sizeof...(Args);
	}

	template<typename Result, typename ...Args>
	inline OtFunctionClass(std::function<Result(Args...)> function) {
		caller = [function]([[maybe_unused]] size_t count, OtObject* parameters) {
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
	inline OtFunctionClass(void (Class::*method)(size_t, OtObject*)) {
		caller = [method](size_t count, OtObject* parameters) {
			OtObjectPointer<Class> object = parameters[0];
			((*object).*method)(count - 1, parameters + 1);
			return nullptr;
		};

		parameterCount = SIZE_MAX;
	}

	template<typename Class>
	inline OtFunctionClass(OtObject (Class::*method)(size_t, OtObject*)) {
		caller = [method](size_t count, OtObject* parameters) {
			OtObjectPointer<Class> object = parameters[0];
			return ((*object).*method)(count - 1, parameters + 1);
		};

		parameterCount = SIZE_MAX;
	}

	template <typename Class, typename Args, std::size_t... I>
	static inline auto methodArgs(OtObject* parameters, std::index_sequence<I...>) {
		return std::make_tuple(
			OtObjectPointer<Class>(parameters[0]),
			OtValue<std::tuple_element_t<I, Args>>::decode(parameters[I + 1])...);
	}

	template<typename Class, typename ...Args>
	inline OtFunctionClass(void (Class::*method)(Args...)) {
		caller = [method]([[maybe_unused]] size_t count, OtObject* parameters) {
			std::apply(
				method,
				methodArgs<Class, typename std::tuple<Args...>>(
					parameters,
					std::make_index_sequence<sizeof...(Args)>()));

			return nullptr;
		};

		parameterCount = sizeof...(Args) + 1;
	}

	template<typename Class, typename Result, typename ...Args>
	inline OtFunctionClass(Result (Class::*method)(Args...)) {
		caller = [method]([[maybe_unused]] size_t count, OtObject* parameters) {
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
	inline OtFunctionClass(Result (Class::*method)(Args...) const) {
		caller = [method]([[maybe_unused]] size_t count, OtObject* parameters) {
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
	// constructors
	friend class OtObjectPointer<OtFunctionClass>;
	OtFunctionClass() = default;

	inline OtFunctionClass(void (*function)(size_t, OtObject*)) {
		caller = [function](size_t count, OtObject* parameters) {
			function(count, parameters);
			return nullptr;
		};

		parameterCount = SIZE_MAX;
	}

	inline OtFunctionClass(OtObject (*function)(size_t, OtObject*)) {
		caller = [function](size_t count, OtObject* parameters) {
			return function(count, parameters);
		};

		parameterCount = SIZE_MAX;
	}

private:
	// data
	std::function<OtObject(size_t, OtObject*)> caller;
	size_t parameterCount = 0;
};
