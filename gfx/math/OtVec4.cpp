//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtInteger.h"
#include "OtLog.h"
#include "OtReal.h"

#include "OtVec4.h"


//
//	OtVec4Class::init
//

void OtVec4Class::init(size_t count, OtObject* parameters) {
	if (count == 4) {
		value = glm::vec4(
			parameters[0]->operator float(),
			parameters[1]->operator float(),
			parameters[2]->operator float(),
			parameters[3]->operator float());

	} else if (count == 1) {
		value = glm::vec4(parameters[0]->operator float());

	} else if (count != 0) {
		OtLogError("[Vec4] constructor expects 0, 1 or 4 arguments (not {})", count);
	}
}


//
//	OtVec4Class::set
//

OtObject OtVec4Class::set(OtID id, OtObject v) {
	if (id == xID) {
		value.x = v->operator float();

	} else if (id == yID) {
		value.y = v->operator float();

	} else if (id == zID) {
		value.z = v->operator float();

	} else if (id == wID) {
		value.w = v->operator float();

	} else {
		OtObjectClass::set(id, v);
	}

	return v;
}


//
//	OtVec4Class::get
//

OtObject OtVec4Class::get(OtID id) {
	if (id == xID) {
		return OtReal::create(value.x);

	} else if (id == yID) {
		return OtReal::create(value.y);

	} else if (id == zID) {
		return OtReal::create(value.z);

	} else if (id == wID) {
		return OtReal::create(value.w);

	} else {
		return OtObjectClass::get(id);
	}
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
