//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtNet
//

class OtHttpClass;
typedef std::shared_ptr<OtHttpClass> OtHttp;

class OtHttpClass : public OtObjectClass {
public:
	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtHttpClass>("Net", OtObjectClass::getMeta());
		}

		return type;
	}
};
