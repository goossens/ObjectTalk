//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtTypeClass::instantiate
//

inline OtObject OtTypeClass::instantiate() {
	OtObject object = constructor();
	object->setType(shared_from_this());
	return object;
}
