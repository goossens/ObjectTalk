//	ObjectTalk Scripting Language
//	Copyright 1993-2021 Johan A. Goossens
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


//
//	OtFunction
//

class OtFunctionClass;
typedef std::shared_ptr<OtFunctionClass> OtFunction;

class OtFunctionClass : public OtPrimitiveClass {
public:
	// constructors
	OtFunctionClass() = default;
	OtFunctionClass(size_t p, OtExecutable e) : parameterCount(p), executable(e) {}

	// call function
	OtObject operator () (OtObject context, size_t count, OtObject* parameters) {
		// sanity check
		if (!executable) {
			OT_EXCEPT("Function not initialized", false);

		} else if (parameterCount != SIZE_MAX && count != parameterCount) {
			OT_EXCEPT("Function expects %d parameters, %d given", parameterCount, count);
		}

		return executable(context, count, parameters);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtFunctionClass>("Function", OtPrimitiveClass::getMeta());
			type->set("__call__", OtFunctionClass::create(&OtFunctionClass::operator ()));
		}

		return type;
	}

	// create a new function
	static OtFunction create(size_t c, OtExecutable e) {
		OtFunction func = std::make_shared<OtFunctionClass>(c, e);
		func->setType(getMeta());
		return func;
	}

	static OtFunction create(OtExecutable e) {
		OtFunction func = std::make_shared<OtFunctionClass>(SIZE_MAX, e);
		func->setType(getMeta());
		return func;
	}

	template<typename Class>
	static OtFunction create(OtObject (Class::*function)(OtObject, size_t, OtObject*)) {
		return create(SIZE_MAX, [function] (OtObject context, size_t c, OtObject* p)->OtObject {
			return (*OtTypeClass::cast<Class>(p[0]).*function)(context, c - 1, p + 1);
		});
	}

protected:
	size_t parameterCount {0};
	OtExecutable executable {nullptr};
};


//
//	Create function object for non-void functions
//

template<typename Function, size_t... I>
OtObject OtFunctionCaller(Function function, std::index_sequence<I...>) {
	return OtFunctionClass::create(sizeof...(I), [function] (OtObject, size_t, OtObject* p)->OtObject {
		return OtObjectCreate(function(*p[I]...));
	});
}

template<typename Result, typename ... Args>
OtObject OtFunctionCreate(Result (*function)(Args...)) {
	return OtFunctionCaller(function, std::index_sequence_for<Args...>());
}

template<typename Result, typename ... Args>
OtObject OtFunctionCreate(std::function<Result (Args...)> function) {
	return OtFunctionCaller(function, std::index_sequence_for<Args...>());
}


//
//	Create function object for void functions
//

template<typename Function, size_t... I>
OtObject OtFunctionCallerVoid(Function function, std::index_sequence<I...>) {
	return OtFunctionClass::create(sizeof...(I), [function] (OtObject, size_t, OtObject* p)->OtObject {
		function(*p[I]...);
		return nullptr;
	});
}

template<typename ... Args>
OtObject OtFunctionCreate(void (*function)(Args...)) {
	return OtFunctionCallerVoid(function, std::index_sequence_for<Args...>());
}

template<typename ... Args>
OtObject OtFunctionCreate(std::function<void (Args...)> function) {
	return OtFunctionCallerVoid(function, std::index_sequence_for<Args...>());
}


//
//	Create function object for non-void member functions
//

template<typename Class, typename Method, size_t... I>
OtObject OtMethodCaller(Method method, std::index_sequence<I...>) {
	return OtFunctionClass::create(sizeof...(I) + 1, [method] (OtObject, size_t, OtObject* p)->OtObject {
		return OtObjectCreate(method(OtTypeClass::cast<Class>(p[0]), *p[I + 1]...));
	});
}

template<typename Result, typename Class, typename... Args>
OtObject OtFunctionCreate(Result (Class::*function)(Args...)) {
	std::function<Result(std::shared_ptr<Class>, Args...)> method = function;
	return OtMethodCaller<Class>(method, std::index_sequence_for<Args...>());
}


//
//	Create function object for void member functions
//

template<typename Class, typename Method, size_t... I>
OtObject OtMethodCallerVoid(Method method, std::index_sequence<I...>) {
	return OtFunctionClass::create(sizeof...(I) + 1, [method] (OtObject, size_t, OtObject* p)->OtObject {
		method(OtTypeClass::cast<Class>(p[0]), *p[I + 1]...);
		return nullptr;
	});
}

template<typename Class, typename... Args>
OtObject OtFunctionCreate(void (Class::*function)(Args...)) {
	std::function<void(std::shared_ptr<Class>, Args...)> method = function;
	return OtMethodCallerVoid<Class>(method, std::index_sequence_for<Args...>());
}


//
//	Overloaded conversion function for ObjectTalk function calls
//

inline OtObject OtObjectCreate(size_t c, OtExecutable e) { return OtFunctionClass::create(c, e); }
inline OtObject OtObjectCreate(OtExecutable e) { return OtFunctionClass::create(e); }
