//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtType
//

class OtTypeClass {
private:
	typedef OtObject(*OtConstructor)();
	typedef std::shared_ptr<OtTypeClass> OtSharedType;

	class OtRegistry {
	public:
		// constructor
		OtRegistry() = default;

		// destructor
		~OtRegistry() {}

		void store(OtSharedType type) {
			types.push_back(type);
		}

		// implemented as singleton
		static OtRegistry& instance() {
			static OtRegistry registry;
			return registry;
		}

		// registry of registered types
		std::vector<OtSharedType> types;
	};

public:
	// constructor
	OtTypeClass(const std::string& n, OtType p, OtConstructor c=nullptr) {
		name = n;
		parent = p;
		constructor = c ? c : parent->constructor;
	}

	// create new instance
	OtObject instantiate();

	// create a sub-type
	OtType subType(const std::string& n) {
		OtSharedType type = std::make_shared<OtTypeClass>(n, this, constructor);
		OtRegistry::instance().store(type);
		return type.get();
	}

	// see if type is kind of
	bool isKindOf(const std::string& className) {
		for (auto p = this; p; p = p->parent) {
			if (p->name == className) {
				return true;
			}
		}

		return false;
	}

	// get information
	std::string getName() { return name; }
	OtType getParent() { return parent; }

	// member access
	bool has(const std::string& name) { return members.count(name) != 0; }
	OtObject set(const std::string& name, OtObject value) { members[name] = value; return value; }
	OtObject get(const std::string& name) { return members.count(name) ? members[name] : nullptr; }

	// create a new type
	template <class CLASS>
	static OtType create(const std::string& name, OtType parent) {
		OtSharedType type = std::make_shared<OtTypeClass>(name, parent, []() {
			return (OtObject) std::make_shared<CLASS>();
		});

		OtRegistry::instance().store(type);
		return type.get();
	}

private:
	// attributes
	std::string name;
	OtType parent;
	std::map<std::string, OtObject> members;
	OtConstructor constructor;
};
