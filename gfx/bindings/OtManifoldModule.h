//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFunction.h"
#include "OtObject.h"
#include "OtValue.h"

#include "OtManifold.h"


//
//	OtManifoldObject
//

class OtManifoldClass;
using OtManifoldObject = OtObjectPointer<OtManifoldClass>;

class OtManifoldClass : public OtObjectClass {
public:
	// constructors
	OtManifoldClass() = default;
	OtManifoldClass(const OtManifold& m) : manifold(m) {}

	// clear the manifold
	inline void clear() { manifold.clear(); }

	// generators
	static inline OtObject cube(float width, float height, float depth, bool center) {
		OtManifold result;
		result.cube(width, height, depth, center);
		return OtManifoldObject::create(result);
	}

	static inline OtObject cylinder(float height, float bottomRadius, float topRadius, int segments, bool center) {
		OtManifold result;
		result.cylinder(height, bottomRadius, topRadius, segments, center);
		return OtManifoldObject::create(result);
	}

	static inline OtObject sphere(float radius, int segments) {
		OtManifold result;
		result.sphere(radius, segments);
		return OtManifoldObject::create(result);
	}

	// combine manifolds
	inline OtObject unionManifolds(OtManifold other) { return OtManifoldObject::create(manifold.unionManifolds(other)); }
	inline OtObject differenceManifolds(OtManifold other) { return OtManifoldObject::create(manifold.differenceManifolds(other)); }
	inline OtObject intersectManifolds(OtManifold other) { return OtManifoldObject::create(manifold.intersectManifolds(other)); }

	// transform manifold
	inline OtObject translate(float x, float y, float z) { return OtManifoldObject::create(manifold.translate(x, y, z)); }
	inline OtObject rotate(float x, float y, float z) { return OtManifoldObject::create(manifold.rotate(x, y, z)); }
	inline OtObject scale(float x, float y, float z) { return OtManifoldObject::create(manifold.scale(x, y, z)); }
	inline OtObject mirror(float x, float y, float z) { return OtManifoldObject::create(manifold.mirror(x, y, z)); }

	// access raw manifold
	inline OtManifold& getManifold() { return manifold; }

	// get type definition
	static inline OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtManifoldClass>("Manifold", OtObjectClass::getMeta());
			type->set("clear", OtFunction::create(&OtManifoldClass::clear));

			type->set("union", OtFunction::create(&OtManifoldClass::unionManifolds));
			type->set("difference", OtFunction::create(&OtManifoldClass::differenceManifolds));
			type->set("intersect", OtFunction::create(&OtManifoldClass::intersectManifolds));

			type->set("translate", OtFunction::create(&OtManifoldClass::translate));
			type->set("rotate", OtFunction::create(&OtManifoldClass::rotate));
			type->set("scale", OtFunction::create(&OtManifoldClass::scale));
			type->set("mirror", OtFunction::create(&OtManifoldClass::mirror));
		}

		return type;
	}

private:
	OtManifold manifold;
};


//
//	Allow OtManifold as an ObjectTalk value
//

template <>
struct OtValue<OtManifold> {
	static inline OtObject encode(OtManifold manifold) {
		return OtManifoldClass(manifold);
	}

	static inline OtManifold decode(OtObject object) {
		if (object.isKindOf<OtManifoldClass>()) {
			return OtManifoldObject(object)->getManifold();

		} else {
			OtLogError("Expected a [Manifold], not a [{}]", object.getTypeName());
			return OtManifold();
		}
	}
};


//
//	Helper functions
//

inline OtObject OtManifoldToObject(OtManifold manifold) {
	return OtValue<OtManifold>::encode(manifold);
}

inline OtManifold OtManifoldFromObject(OtObject object) {
	return OtValue<OtManifold>::decode(object);
}

inline bool OtManifoldValidateObject(OtObject object) {
	return object.isKindOf<OtManifoldClass>();
}
