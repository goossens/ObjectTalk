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
//	OtTypeClass 
//

class OtTypeClass : public std::enable_shared_from_this<OtTypeClass> {
public:
	// constructor
	OtTypeClass(const std::wstring& n, OtType p, OtConstructor c=nullptr) {
		name = n;
		parent = p;
		constructor = c ? c : parent->constructor;
	}

	// create new instance
	OtObject instantiate();

	// create a sub-type
	OtType subType(const std::wstring& n, OtConstructor c=nullptr) {
		return std::make_shared<OtTypeClass>(n, getSharedPtr(), c);
	}

	// get shared pointer
	OtType getSharedPtr() { return shared_from_this(); }

	// see if type is kind of
	bool isKindOf(const std::wstring& className) {
		for (auto p = getSharedPtr(); p; p = p->parent) {
			if (p->name == className) {
				return true;
			}
		}

		return false;
	}

	bool isKindOf(OtType metaclass)  { return isKindOf(metaclass->name); }

	// get information
	std::wstring getName() { return name; }
	OtType getParent() { return parent; }

	// member access
	bool has(const std::wstring& name) { return get(name) != nullptr; }
	OtObject set(const std::wstring& name, OtObject value) { members[name] = value; return value; }
	OtObject get(const std::wstring& name) { return members.count(name) ? members[name] : nullptr; }

	// create a new type
	template <class T>
	static OtType create(const std::wstring& name, OtType parent=nullptr) {
		return std::make_shared<OtTypeClass>(name, parent, []() { return std::make_shared<T>(); });
	}

	// cast to different type
	template <class T>
	static std::shared_ptr<T> cast(OtObject object) { return std::dynamic_pointer_cast<T>(object); }

private:
	// attributes
	std::wstring name;
	OtType parent;
	std::map<std::wstring, OtObject> members;
	OtConstructor constructor;
};
