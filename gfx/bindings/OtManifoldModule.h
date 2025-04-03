//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

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
	// clear the manifold
	inline void clear() { manifold.clear(); }

	// shape I/O
	inline void save(const std::string& filepath) { manifold.save(filepath); }
	inline void load(const std::string& filepath) { manifold.load(filepath); }

	// generators
	static inline OtObject cube(float width, float height, float depth, bool center) {
		return OtManifoldObject::create(OtManifold::cube(width, height, depth, center));
	}

	static inline OtObject cylinder(float height, float bottomRadius, float topRadius, int segments, bool center) {
		return OtManifoldObject::create(OtManifold::cylinder(height, bottomRadius, topRadius, segments, center));
	}

	static inline OtObject sphere(float radius, int segments) {
		return OtManifoldObject::create(OtManifold::sphere(radius, segments));
	}

	static OtObject compose(OtObject array);

	static inline OtObject extrude(OtShape shape, float height, int divisions, float twistDegrees, float scaleTop, float tolerance) {
		return OtManifoldObject::create(OtManifold::extrude(shape, height, divisions, twistDegrees, scaleTop, tolerance));
	}

	static inline OtObject revolve(OtShape shape, int segments, float revolveDegrees, float tolerance) {
		return OtManifoldObject::create(OtManifold::revolve(shape, segments, revolveDegrees, tolerance));
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

	inline OtObject hull() { return OtManifoldObject::create(manifold.hull()); }

	// get manifold information
	inline size_t getVertexCount() { return manifold.getVertexCount(); }
	inline size_t getTriangleCount() { return manifold.getTriangleCount(); }

	// access raw manifold
	inline OtManifold& getManifold() { return manifold; }

	// get type definition
	static OtType getMeta();

protected:
	// constructors
	friend class OtObjectPointer<OtManifoldClass>;
	OtManifoldClass() = default;
	OtManifoldClass(const OtManifold& m) : manifold(m) {}

private:
	// data
	OtManifold manifold;
};


//
//	Allow OtManifold as an ObjectTalk value
//

template <>
struct OtValue<OtManifold> {
	static inline OtObject encode(OtManifold manifold) {
		return OtManifoldObject::create(manifold);
	}

	static inline OtManifold decode(OtObject object) {
		object.expect<OtManifoldClass>("Manifold");
		return OtManifoldObject(object)->getManifold();
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
