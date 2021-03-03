//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtType
//

class OtTypeClass;
typedef std::shared_ptr<OtTypeClass> OtType;


//
//	OtObject
//

class OtObjectClass;
typedef std::shared_ptr<OtObjectClass> OtObject;


//
//	OtConstructor
//

typedef std::function<OtObject ()> OtConstructor;


//
//	OtExecutable
//

typedef std::function<OtObject (OtObject context, size_t count, OtObject* parameters)> OtExecutable;
