//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtObject.h"
#include "OtValue.h"

#include "OtGeometry.h"
#include "OtPrimitiveBase.h"


//
//	OtGeometryObject
//

class OtGeometryClass;
using OtGeometryObject = OtObjectPointer<OtGeometryClass>;

class OtGeometryClass : public OtObjectClass {
public:
	// clear the geometry
	inline void clear() { geometry.clear(); }

	// geometry I/O
	inline void save(const std::string& filepath) { geometry.save(filepath); }
	inline void load(const std::string& filepath) { geometry.load(filepath); }

	// add other geometries
	void add(size_t count, OtObject* parameters);

	// optimize geometry
	inline void optimize() { geometry.optimize(); }

	// access raw geometry
	inline OtGeometry& getGeometry() { return geometry; }

	// generators
	static OtObject circle(size_t count, OtObject* parameters);
	static OtObject cube(size_t count, OtObject* parameters);
	static OtObject cylinder(size_t count, OtObject* parameters);
	static OtObject dodecahedron();
	static OtObject grid(size_t count, OtObject* parameters);
	static OtObject icosahedron();
	static OtObject octahedron();
	static OtObject plane(size_t count, OtObject* parameters);
	static OtObject sphere(size_t count, OtObject* parameters);
	static OtObject tetrahedron();
	static OtObject torus(size_t count, OtObject* parameters);

	// get type definition
	static OtType getMeta();

protected:
	// constructors
	friend class OtObjectPointer<OtGeometryClass>;
	OtGeometryClass() = default;
	OtGeometryClass(const OtGeometry& m) : geometry(m) {}

private:
	// data
	OtGeometry geometry;

	// local functions
	static inline OtObject createGeometry(OtPrimitiveBase& primitive) {
		auto mesh = primitive.createMesh();
		return OtGeometryObject::create(mesh);
	}
};


//
//	Allow OtGeometry as an ObjectTalk value
//

template <>
struct OtValue<OtGeometry> {
	static inline OtObject encode(OtGeometry geometry) {
		return OtGeometryObject::create(geometry);
	}

	static inline OtGeometry decode(OtObject object) {
		object.expect<OtGeometryClass>("Geometry");
		return OtGeometryObject(object)->getGeometry();
	}
};


//
//	Helper functions
//

inline OtObject OtGeometryToObject(OtGeometry geometry) {
	return OtValue<OtGeometry>::encode(geometry);
}

inline OtGeometry OtGeometryFromObject(OtObject object) {
	return OtValue<OtGeometry>::decode(object);
}

inline bool OtGeometryValidateObject(OtObject object) {
	return object.isKindOf<OtGeometryClass>();
}
