//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtFunction
//

class OtFunctionClass;
typedef std::shared_ptr<OtFunctionClass> OtFunction;

class OtFunctionClass : public OtPrimitiveClass {
	typedef OtObject (*OtInvoke)(void* target, OtObject context, size_t count, OtObject* parameters);

public:
	// constructors
	OtFunctionClass() = default;
	OtFunctionClass(OtInvoke i, void* t, size_t p) : invoker(i), target(t), parameterCount(p) {}

	// destructor
	~OtFunctionClass() {
		free(target);
	}

	// call function
	OtObject operator () (OtObject context, size_t count, OtObject* parameters) {
		// sanity check
		if (parameterCount != SIZE_MAX && count != parameterCount) {
			throw OtException(OtFormat("Function expects %d parameters, %d given", parameterCount, count));
		}

		return (*invoker)(target, context, count, parameters);
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

	// invoke functions by type
	template<typename FUNCTION, size_t... I>
	inline static OtObject invokeVoidFunction(void* t, OtObject* p, std::index_sequence<I...>) {
		(*((FUNCTION*) t))(*p[I]...);
		return nullptr;
	}

	template<typename FUNCTION, size_t... I>
	inline static OtObject invokeFunction(void* t, OtObject* p, std::index_sequence<I...>) {
		return OtObjectCreate((*((FUNCTION*) t))(*p[I]...));
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

	// create new function bindings by function type
	static OtFunction create(OtObject (*function)(OtObject, size_t, OtObject*)) {
		typedef decltype(function) FUNCTION;
		FUNCTION* target = (FUNCTION*) malloc(sizeof(FUNCTION));
		*target = function;

		struct Helper {
			static OtObject invoke(void* t, OtObject c, size_t n, OtObject* p) {
				return (*((FUNCTION*) t))(c, n, p);
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
			static OtObject invoke(void* t, OtObject c, size_t n, OtObject* p) {
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
			static OtObject invoke(void* t, OtObject c, size_t n, OtObject* p) {
				return invokeVoidFunction<FUNCTION>(t, p, std::index_sequence_for<ARGS...>());
			}
		};

		return createWithType(std::make_shared<OtFunctionClass>(&Helper::invoke, target, sizeof...(ARGS)));
	}

	template<typename CLASS>
	static OtFunction create(OtObject (CLASS::*method)(OtObject, size_t, OtObject*)) {
		typedef decltype(method) METHOD;
		METHOD* target = (METHOD*) malloc(sizeof(METHOD));
		*target = method;

		struct Helper {
			static OtObject invoke(void* t, OtObject c, size_t n, OtObject* p) {
				auto object = p[0]->cast<CLASS>();
				auto method = *((METHOD*) t);
				return ((*object).*method)(c, n - 1, p + 1);
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
			static OtObject invoke(void* t, OtObject c, size_t n, OtObject* p) {
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
			static OtObject invoke(void* t, OtObject c, size_t n, OtObject* p) {
				return invokeMethod<CLASS, METHOD>(t, p, std::index_sequence_for<ARGS...>());
			}
		};

		return createWithType(std::make_shared<OtFunctionClass>(&Helper::invoke, target, sizeof...(ARGS) + 1));
	}

protected:
	OtInvoke invoker {nullptr};
	void* target {nullptr};
	size_t parameterCount {0};
};
