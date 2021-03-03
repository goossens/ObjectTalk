//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtSystem
//

class OtSystemClass;
typedef std::shared_ptr<OtSystemClass> OtSystem;

class OtSystemClass : public OtObjectClass {
public:
	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtSystemClass>("System", OtObjectClass::getMeta());
		}

		return type;
	}
};
