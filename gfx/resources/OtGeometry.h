//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <string>

#include "OtMesh.h"


//
//	OtGeometry
//

class OtGeometry {
public:
	// constructors
	OtGeometry();
	inline OtGeometry(std::shared_ptr<OtMesh> m) : mesh(m) {}
	OtGeometry(const std::string& path);

	// clear the geometry
	void clear();

	// see if geometry is valid
	inline bool isValid() { return mesh->isValid(); }

	// access the mesh
	void setMesh(std::shared_ptr<OtMesh> mesh);
	OtMesh& getMesh() { return* mesh.get(); }
	void cloneMesh(const OtGeometry& geometry);

	// load/save the geometry
	inline void load(const std::string& path) { mesh->load(path); }
	inline void save(const std::string& path) { mesh->save(path); }

	// access bounding box
	OtAABB& getAABB() { return mesh->getAABB(); }

	// submit to GPU
	inline void submitTriangles() { mesh->submitTriangles(); }
	inline void submitLines() { mesh->submitLines(); }

	// version management
	inline void setVersion(int v) { version = v; }
	inline int getVersion() { return version; }
	inline void incrementVersion() { version++; }

	// see if geometries are identical
	inline bool operator==(OtGeometry& rhs) {
		return mesh == rhs.mesh && version == rhs.version;
	}

	inline bool operator!=(OtGeometry& rhs) {
		return !operator==(rhs);
	}

protected:
	// properties
	std::shared_ptr<OtMesh> mesh;
	int version = 0;
};
