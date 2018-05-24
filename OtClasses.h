//	ObjectTalk Scripting Language
//	Copyright 1993-2018 Johan A. Goossens
//
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.


#pragma once


//
//  Include files
//

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "OtNucleus.h"


//
//  Create primitive values
//

OtValue OtValueCreate();
inline OtValue OtValueCreate(OtValue value) { return value; }
OtValue OtValueCreate(bool boolean);
OtValue OtValueCreate(long integer);
OtValue OtValueCreate(size_t integer);
OtValue OtValueCreate(double real);
OtValue OtValueCreate(const std::string& string);
OtValue OtValueCreate(OtType type);
OtValue OtValueCreate(size_t count, OtExecutable executable);
OtValue OtValueCreate(size_t count, OtCode code, const std::vector<std::string>& names);


//
//  Create raw function values
//

inline OtValue OtFunctionCreateRaw(size_t count, OtExecutable executable) { return OtValueCreate(count, executable); }
inline OtValue OtFunctionCreateRaw(OtExecutable executable) { return OtValueCreate(SIZE_MAX, executable); }

template<typename Class>
OtValue OtFunctionCreateRaw(OtValue (Class::*function)(size_t count, OtValue* parameters))
{
	return OtValueCreate(SIZE_MAX, [function] (OtValue, size_t c, OtValue* p)->OtValue
	{
		return (*OtTypeCast<Class>(p[0]).*function)(c - 1, p + 1);
	});
}

template<typename Class>
OtValue OtFunctionCreateRaw(OtValue (Class::*function)(OtValue, size_t, OtValue*))
{
	return OtValueCreate(SIZE_MAX, [function] (OtValue context, size_t c, OtValue* p)->OtValue
	{
		return (*OtTypeCast<Class>(p[0]).*function)(context, c - 1, p + 1);
	});
}


//
//  Create function values for non-void functions
//

template<typename Function, size_t... I>
OtValue OtFunctionCaller(Function function, std::index_sequence<I...>)
{
	return OtFunctionCreateRaw(sizeof...(I), [function] (OtValue, size_t, OtValue* p)->OtValue
	{
		return OtValueCreate(function(*p[I]...));
	});
}

template<typename Result, typename ... Args>
OtValue OtFunctionCreate(Result (*function)(Args...))
{
	return OtFunctionCaller(function, std::index_sequence_for<Args...>());
}

template<typename Result, typename ... Args>
OtValue OtFunctionCreate(std::function<Result (Args...)> function)
{
	return OtFunctionCaller(function, std::index_sequence_for<Args...>());
}


//
//  Create function values for void functions
//

template<typename Function, size_t... I>
OtValue OtFunctionCallerVoid(Function function, std::index_sequence<I...>)
{
	return OtFunctionCreateRaw(sizeof...(I), [function] (OtValue, size_t, OtValue* p)->OtValue
	{
		function(*p[I]...);
		return nullptr;
	});
}

template<typename ... Args>
OtValue OtFunctionCreate(void (*function)(Args...))
{
	return OtFunctionCallerVoid(function, std::index_sequence_for<Args...>());
}

template<typename ... Args>
OtValue OtFunctionCreate(std::function<void (Args...)> function)
{
	return OtFunctionCallerVoid(function, std::index_sequence_for<Args...>());
}


//
//  Create function values for non-void member functions
//

template<typename Class, typename Method, size_t... I>
OtValue OtMethodCaller(Method method, std::index_sequence<I...>)
{
	return OtFunctionCreateRaw(sizeof...(I) + 1, [method] (OtValue, size_t, OtValue* p)->OtValue
	{
		return OtValueCreate(method(OtTypeCast<Class>(p[0]), *p[I + 1]...));
	});
}

template<typename Result, typename Class, typename... Args>
OtValue OtFunctionCreate(Result (Class::*function)(Args...))
{
	std::function<Result(std::shared_ptr<Class>, Args...)> method = function;
	return OtMethodCaller<Class>(method, std::index_sequence_for<Args...>());
}


//
//  Create function values for void member functions
//

template<typename Class, typename Method, size_t... I>
OtValue OtMethodCallerVoid(Method method, std::index_sequence<I...>)
{
	return OtFunctionCreateRaw(sizeof...(I) + 1, [method] (OtValue, size_t, OtValue* p)->OtValue
	{
		method(OtTypeCast<Class>(p[0]), *p[I + 1]...);
		return nullptr;
	});
}

template<typename Class, typename... Args>
OtValue OtFunctionCreate(void (Class::*function)(Args...))
{
	std::function<void(std::shared_ptr<Class>, Args...)> method = function;
	return OtMethodCallerVoid<Class>(method, std::index_sequence_for<Args...>());
}


//
//  Create bound function
//

OtValue OtBoundFunctionCreate(OtValue object, OtValue function);


//
//  Create collection values
//

OtValue OtArrayCreate();
OtValue OtArrayCreate(size_t count, OtValue* values);

OtValue OtDictCreate();
OtValue OtDictCreate(size_t count, OtValue* values);


//
//	Create IO values
//


OtValue OtFileCreate();
OtValue OtHttpCreate();


//
//  Create reference values
//

OtValue OtContextReferenceCreate(const std::string& member);
OtValue OtMemberReferenceCreate(OtValue object, const std::string& member);
OtValue OtArrayReferenceCreate(OtValue array, size_t index);
OtValue OtDictReferenceCreate(OtValue dict, const std::string& index);


//
//  Create class values
//

OtValue OtObjectClassCreate();

OtValue OtPrimitiveClassCreate();
OtValue OtBooleanClassCreate();
OtValue OtIntegerClassCreate();
OtValue OtRealClassCreate();
OtValue OtStringClassCreate();
OtValue OtFunctionClassCreate();

OtValue OtCollectionClassCreate();
OtValue OtArrayClassCreate();
OtValue OtDictClassCreate();

OtValue OtIOClassCreate();
OtValue OtFileClassCreate();
OtValue OtHttpClassCreate();

OtValue OtInternalClassCreate();
OtValue OtClassClassCreate();
OtValue OtMemberReferenceClassCreate();
OtValue OtArrayReferenceClassCreate();
OtValue OtDictReferenceClassCreate();


//
//	Include class definitions
//

#include "OtPrimitives.h"
#include "OtCollections.h"
#include "OtIO.h"
#include "OtInternal.h"
