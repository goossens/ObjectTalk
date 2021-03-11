//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtCodeFunction
//

class OtCodeFunctionClass;
typedef std::shared_ptr<OtCodeFunctionClass> OtCodeFunction;


class OtCodeFunctionClass : public OtInternalClass {
public:
	// constructor
	OtCodeFunctionClass() = default;
	OtCodeFunctionClass(size_t p, const std::vector<std::string>& n, std::weak_ptr<OtCodeClass> c, size_t s, size_t e)
		: parameterCount(p), names(n), code(c), start(s), end(e) {}

	// call code
	OtObject operator () (OtContext context, size_t count, OtObject* parameters) {
		// sanity check
		if (parameterCount != SIZE_MAX && parameterCount != count) {
			OT_EXCEPT("Function expects %d parameters, %d given", parameterCount, count);
		}

		// create local context
		OtContext local = OtContextClass::create();
		local->setParent(context);

		// set calling parameters
		local->set("args", OtArrayClass::create(count, parameters));

		for (auto c= 0; c < count; c++) {
			local->set(names[c], parameters[c]);
		}

		return code.lock()->operator ()(local, start, end);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtCodeFunctionClass>("CodeFunction", OtInternalClass::getMeta());
			type->set("__call__", OtFunctionClass::create(&OtCodeFunctionClass::operator ()));
		}

		return type;
	}

	// create a new object
	static OtCodeFunction create(size_t count, const std::vector<std::string>& names, std::weak_ptr<OtCodeClass> code, size_t start, size_t end) {
		OtCodeFunction func = std::make_shared<OtCodeFunctionClass>(count, names, code, start, end);
		func->setType(getMeta());
		return func;
	}

private:
	size_t parameterCount;
	std::vector<std::string> names;
	std::weak_ptr<OtCodeClass> code;
	size_t start;
	size_t end;
};
