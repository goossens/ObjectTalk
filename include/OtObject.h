//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtObject
//

class OtObjectClass : public std::enable_shared_from_this<OtObjectClass> {
public:
	// constructor
	OtObjectClass() = default;

	// destructor
	virtual ~OtObjectClass() {}

	// type access
	void setType(OtType t) { type = t; }
	OtType getType() { return type; }

	// see if object "is kind of"
	bool isKindOf(const std::string& className) { return type->isKindOf(className); }

	// default conversion operators
	virtual operator bool() { return false; }
	virtual operator long() { return 0; }
	virtual operator size_t() { return 0; }
	virtual operator double() { return 0.0; }
	virtual operator std::string() { return ""; }
	virtual operator OtObject() { return shared_from_this(); }

	// get object's JSON representation
	virtual std::string json() { return operator std::string(); }

	// methods to allow binding to virtual member functions
	bool toBoolean() { return operator bool(); }
	long toInteger() { return operator long(); }
	double toReal() { return operator double(); }
	std::string toString() { return operator std::string(); }
	std::string toJSON() { return json(); }

	// get shared pointer
	OtObject getSharedPtr() { return shared_from_this(); }

	// member acccess
	bool hasMember(const std::string& name) {
		return members && members->count(name);
	}

	OtObject getMember(const std::string& name) {
		return members->operator [] (name);
	}

	virtual bool has(const std::string& name) {
		if (hasMember(name)) {
			return true;
		}

		for (auto t = type; t; t = t->getParent()) {
			if (t->has(name)) {
				return true;
			}
		}

		return false;
	}

	virtual OtObject set(const std::string& name, OtObject value) {
		if (!members) {
			members = std::make_shared<std::unordered_map<std::string, OtObject>>();
		}

		members->operator [] (name) = value;
		return value;
	}

	virtual OtObject get(const std::string& name) {
		if (hasMember(name)) {
			return getMember(name);
		}

		for (auto t = type; t; t = t->getParent()) {
			if (t->has(name)) {
				return t->get(name);
			}
		}

		OT_EXCEPT("Unknown member [%s] in instance of class [%s]", name.c_str(), type->getName().c_str());
		return nullptr;
	}

	virtual void eraseMember(const std::string& name) { if (members) members->erase(name); }
	virtual void clearMembers() { members = nullptr; }

	// support member operator
	OtObject member(const std::string& name);

	// "call" object (context, count, parameters)
	virtual OtObject operator () (OtContext, size_t, OtObject*) { return nullptr; }

	// "call" named object member
	OtObject method(const std::string& m, OtContext c, size_t n, OtObject* p) {
		OtObject pars[n + 1];
		pars[0] = shared_from_this();
		std::copy(p, p + n, pars + 1);
		return get(m)->operator ()(c, n + 1, pars);
	}

	OtClass getClass();

	// cast shared pointer to specified type
	template <typename CLASS>
	std::shared_ptr<CLASS> cast() { return std::dynamic_pointer_cast<CLASS>(shared_from_this()); }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtObject create() {
		OtObject object = std::make_shared<OtObjectClass>();
		object->setType(getMeta());
		return object;
	}

protected:
	// object type
	OtType type;

	// members
	std::shared_ptr<std::unordered_map<std::string, OtObject>> members;
};


//
//	Overloaded conversion function for ObjectTalk function calls
//

inline OtObject OtObjectCreate() { return OtObjectClass::create(); }
inline OtObject OtObjectCreate(OtObject value) { return value; }


//
//	Forward reference
//

inline OtObject OtObjectCreate(bool value);
inline OtObject OtObjectCreate(long value);
inline OtObject OtObjectCreate(size_t value);
inline OtObject OtObjectCreate(float value);
inline OtObject OtObjectCreate(double value);
inline OtObject OtObjectCreate(const std::string& value);
