//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtContext
//

class OtContextClass : public OtInternalClass {
public:
	// constructor
	OtContextClass() = default;

	// parent access
	void setParent(OtContext v) { parent = v; }
	OtContext getParent() { return parent.lock(); }

	// member acccess
	bool virtual has(const std::string& name) {
		if (hasMember(name)) {
			return true;
		}

		for (auto p = parent.lock(); p; p = p->getParent()) {
			if (p->hasMember(name)) {
				return true;
			}
		}

		for (auto t = type; t; t = t->getParent()) {
			if (t->has(name)) {
				return true;
			}
		}

		return false;
	}

	OtObject get(const std::string& name) {
		if (hasMember(name)) {
			return getMember(name);
		}

		for (auto p = parent.lock(); p; p = p->getParent()) {
			if (p->hasMember(name)) {
				return p->getMember(name);
			}
		}

		for (auto t = type; t; t = t->getParent()) {
			if (t->has(name)) {
				return t->get(name);
			}
		}

		throw OtException(OtFormat("Unknown member [%s] in instance of class [%s]", name.c_str(), type->getName().c_str()));
		return nullptr;
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtContextClass>("Context", OtInternalClass::getMeta());
		}

		return type;
	}

	// create a new object
	static OtContext create() {
		OtContext context = std::make_shared<OtContextClass>();
		context->setType(getMeta());
		return context;
	}

private:
		// parent in context chain
		std::weak_ptr<OtContextClass> parent;
};
