//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
//	OtTorusPrimitive
//

class OtTorusPrimitive : public OtPrimitiveBase {
public:
	// update attributes
	inline void setRadius(float r) { radius = r; }
	inline void setTubeRadius(float tr) { tubeRadius = tr; }
	inline void setRadialSegments(int rs) { radialSegments = rs; }
	inline void setTubularSegments(int ts) { tubularSegments = ts; }
	inline void setRadialStart(float rs) { radialStart = rs; }
	inline void setRadialLength(float rl) { radialLength = rl; }
	inline void setTubularStart(float ts) { tubularStart = ts; }
	inline void setTubularLength(float tl) { tubularLength = tl; }

	inline float getRadius() { return radius; }
	inline float getTubeRadius() { return tubeRadius; }
	inline int getRadialSegments() { return radialSegments; }
	inline int getTubularSegments() { return tubularSegments; }
	inline float getRadialStart() { return radialStart; }
	inline float getRadialLength() { return radialLength; }
	inline float getTubularStart() { return tubularStart; }
	inline float getTubularLength() { return tubularLength; }

	// UI to change primitive's properties
	bool renderUI() override;

	// (de)serialize primitive
	nlohmann::json serialize(std::string* basedir) override;
	void deserialize(nlohmann::json data, std::string* basedir) override;

	// get type name of primitive
	const char* getTypeName() override { return name; }

	// primitive name
	static constexpr char const* name = "Torus";

private:
	// generate mesh
	void createMesh(OtMesh* mesh) override;

	// attributes
	float radius = 1.0f;
	float tubeRadius = 0.4f;
	int radialSegments = 64;
	int tubularSegments = 32;
	float radialStart = 0.0f;
	float radialLength = 360.0f;
	float tubularStart = 0.0f;
	float tubularLength = 360.0f;
};
