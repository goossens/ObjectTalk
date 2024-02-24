//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "nlohmann/json_fwd.hpp"

#include "OtPrimitiveBase.h"


//
//	OtCubePrimitive
//

class OtCubePrimitive : public OtPrimitiveBase {
public:
	// access properties
	inline void setWidthSegments(int ws) { widthSegments = ws; }
	inline void setHeightSegments(int hs) { heightSegments = hs; }
	inline void setDepthSegments(int ds) { depthSegments = ds; }

	inline int getWidthSegments() { return widthSegments; }
	inline int getHeightSegments() { return heightSegments; }
	inline int getDepthSegments() { return depthSegments; }

	// UI to change primitive's properties
	bool renderUI() override;

	// (de)serialize primitive
	nlohmann::json serialize(std::string* basedir) override;
	void deserialize(nlohmann::json data, std::string* basedir) override;

	// get type name of primitive
	const char* getTypeName() override { return name; }

	// primitive name
	static constexpr char const* name = "Cube";

private:
	// generate mesh
	void createMesh(OtMesh* mesh) override;

	// create one side of the BoxGeometry
	void buildPlane(OtMesh* mesh, int udir, int vdir, float w, float h, float d, int gridX, int gridY, glm::vec3 (*cb)(float, float, float));

	// attributes
	int widthSegments = 1;
	int heightSegments = 1;
	int depthSegments = 1;
};
