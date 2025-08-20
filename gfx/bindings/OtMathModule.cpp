//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"
#include "OtFunction.h"
#include "OtModule.h"

#include "OtMathModule.h"


//
//	OtVec2Class::getMeta
//

OtType OtVec2Class::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtVec2Class>("Vec2", OtObjectClass::getMeta());
		type->set("__init__", OtFunction::create(&OtVec2Class::init));

		type->set("__add__", OtFunction::create(&OtVec2Class::add));
		type->set("__sub__", OtFunction::create(&OtVec2Class::subtract));
		type->set("__mul__", OtFunction::create(&OtVec2Class::multiply));
		type->set("__div__", OtFunction::create(&OtVec2Class::divide));
		type->set("__mod__", OtFunction::create(&OtVec2Class::modulo));

		type->set("__eq__", OtFunction::create(&OtVec2Class::equal));
		type->set("__ne__", OtFunction::create(&OtVec2Class::notEqual));

		type->set("length", OtFunction::create(&OtVec2Class::length));
		type->set("distance", OtFunction::create(&OtVec2Class::distance));

		type->set("min", OtFunction::create(&OtVec2Class::min));
		type->set("max", OtFunction::create(&OtVec2Class::max));

		type->set("abs", OtFunction::create(&OtVec2Class::abs));
		type->set("ceil", OtFunction::create(&OtVec2Class::floor));
		type->set("floor", OtFunction::create(&OtVec2Class::floor));
		type->set("fract", OtFunction::create(&OtVec2Class::fract));
		type->set("normalize", OtFunction::create(&OtVec2Class::normalize));
		type->set("round", OtFunction::create(&OtVec2Class::floor));
		type->set("sign", OtFunction::create(&OtVec2Class::floor));
		type->set("trunc", OtFunction::create(&OtVec2Class::floor));

		type->set("clamp", OtFunction::create(&OtVec2Class::clamp));
		type->set("smoothstep", OtFunction::create(&OtVec2Class::smoothstep));
		type->set("step", OtFunction::create(&OtVec2Class::step));

		type->set("lerp", OtFunction::create(&OtVec2Class::lerp));
	}

	return type;
}


//
//	OtVec4Class::getMeta
//

OtType OtVec4Class::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtVec4Class>("Vec4", OtObjectClass::getMeta());
		type->set("__init__", OtFunction::create(&OtVec4Class::init));

		type->set("__add__", OtFunction::create(&OtVec4Class::add));
		type->set("__sub__", OtFunction::create(&OtVec4Class::subtract));
		type->set("__mul__", OtFunction::create(&OtVec4Class::multiply));
		type->set("__div__", OtFunction::create(&OtVec4Class::divide));
		type->set("__mod__", OtFunction::create(&OtVec4Class::modulo));

		type->set("length", OtFunction::create(&OtVec4Class::length));
		type->set("distance", OtFunction::create(&OtVec4Class::distance));

		type->set("min", OtFunction::create(&OtVec4Class::min));
		type->set("max", OtFunction::create(&OtVec4Class::max));

		type->set("abs", OtFunction::create(&OtVec4Class::abs));
		type->set("ceil", OtFunction::create(&OtVec4Class::floor));
		type->set("floor", OtFunction::create(&OtVec4Class::floor));
		type->set("fract", OtFunction::create(&OtVec4Class::fract));
		type->set("normalize", OtFunction::create(&OtVec4Class::normalize));
		type->set("round", OtFunction::create(&OtVec4Class::floor));
		type->set("sign", OtFunction::create(&OtVec4Class::floor));
		type->set("trunc", OtFunction::create(&OtVec4Class::floor));

		type->set("clamp", OtFunction::create(&OtVec4Class::clamp));
		type->set("smoothstep", OtFunction::create(&OtVec4Class::smoothstep));
		type->set("step", OtFunction::create(&OtVec4Class::step));

		type->set("lerp", OtFunction::create(&OtVec4Class::lerp));
	}

	return type;
}


//
//	OtVec3Class::getMeta
//

OtType OtVec3Class::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtVec3Class>("Vec3", OtObjectClass::getMeta());
		type->set("__init__", OtFunction::create(&OtVec3Class::init));

		type->set("__add__", OtFunction::create(&OtVec3Class::add));
		type->set("__sub__", OtFunction::create(&OtVec3Class::subtract));
		type->set("__mul__", OtFunction::create(&OtVec3Class::multiply));
		type->set("__div__", OtFunction::create(&OtVec3Class::divide));
		type->set("__mod__", OtFunction::create(&OtVec3Class::modulo));

		type->set("__eq__", OtFunction::create(&OtVec3Class::equal));
		type->set("__ne__", OtFunction::create(&OtVec3Class::notEqual));

		type->set("length", OtFunction::create(&OtVec3Class::length));
		type->set("distance", OtFunction::create(&OtVec3Class::distance));
		type->set("dot", OtFunction::create(&OtVec3Class::dot));
		type->set("cross", OtFunction::create(&OtVec3Class::cross));

		type->set("min", OtFunction::create(&OtVec3Class::min));
		type->set("max", OtFunction::create(&OtVec3Class::max));

		type->set("abs", OtFunction::create(&OtVec3Class::abs));
		type->set("ceil", OtFunction::create(&OtVec3Class::floor));
		type->set("floor", OtFunction::create(&OtVec3Class::floor));
		type->set("fract", OtFunction::create(&OtVec3Class::fract));
		type->set("normalize", OtFunction::create(&OtVec3Class::normalize));
		type->set("round", OtFunction::create(&OtVec3Class::floor));
		type->set("sign", OtFunction::create(&OtVec3Class::floor));
		type->set("trunc", OtFunction::create(&OtVec3Class::floor));

		type->set("clamp", OtFunction::create(&OtVec3Class::clamp));
		type->set("smoothstep", OtFunction::create(&OtVec3Class::smoothstep));
		type->set("step", OtFunction::create(&OtVec3Class::step));

		type->set("lerp", OtFunction::create(&OtVec3Class::lerp));
	}

	return type;
}


//
//	OtMat4Class::getMeta
//

OtType OtMat4Class::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtMat4Class>("Mat4", OtObjectClass::getMeta());
		type->set("__init__", OtFunction::create(&OtMat4Class::init));

		type->set("__add__", OtFunction::create(&OtMat4Class::add));
		type->set("__sub__", OtFunction::create(&OtMat4Class::subtract));
		type->set("__mul__", OtFunction::create(&OtMat4Class::multiply));

		type->set("__eq__", OtFunction::create(&OtMat4Class::equal));
		type->set("__ne__", OtFunction::create(&OtMat4Class::notEqual));

		type->set("inverse", OtFunction::create(&OtMat4Class::inverse));
		type->set("transpose", OtFunction::create(&OtMat4Class::transpose));

		type->set("translate", OtFunction::create(&OtMat4Class::translate));
		type->set("rotate", OtFunction::create(&OtMat4Class::rotate));
		type->set("scale", OtFunction::create(&OtMat4Class::scale));
	}

	return type;
}


//
//	Module registration
//

static OtModuleRegistration registration{"math", [](OtModule module) {
	module->set("Vec2", OtClass::create(OtVec2Class::getMeta()));
	module->set("Vec3", OtClass::create(OtVec3Class::getMeta()));
	module->set("Vec4", OtClass::create(OtVec4Class::getMeta()));
	module->set("Mat4", OtClass::create(OtVec4Class::getMeta()));
}};
