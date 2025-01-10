//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
//	OtSpherePrimitive
//

class OtSpherePrimitive : public OtPrimitiveBase {
public:
	// update attributes
	inline void setRadius(float r) { radius = r; }
	inline void setPhiSegments(int ps) { phiSegments = ps; }
	inline void setPhiStart(float ps) { phiStart = ps; }
	inline void setPhiLength(float pl) { phiLength = pl; }
	inline void setThetaSegments(int ts) { thetaSegments = ts; }
	inline void setThetaStart(float ts) { thetaStart = ts; }
	inline void setThetaLength(float tl) { thetaLength = tl; }

	inline float getRadius() { return radius; }
	inline int getPhiSegments() { return phiSegments; }
	inline float getPhiStart() { return phiStart; }
	inline float getsetPhiLength() { return phiLength; }
	inline int getThetaSegments() { return thetaSegments; }
	inline float getThetaStart() { return thetaStart; }
	inline float getThetaLength() { return thetaLength; }

	// UI to change primitive's properties
	bool renderUI() override;

	// (de)serialize primitive
	nlohmann::json serialize(std::string* basedir) override;
	void deserialize(nlohmann::json data, std::string* basedir) override;

	// get type name of primitive
	const char* getTypeName() override { return name; }

	// primitive name
	static constexpr char const* name = "Sphere";

private:
	// generate mesh
	void createMesh(OtMesh* mesh) override;

	// attributes
	float radius = 1.0f;
	int phiSegments = 32;
	float phiStart = 0.0f;
	float phiLength = 360.0f;
	int thetaSegments = 64;
	float thetaStart = 0.0f;
	float thetaLength = 180.0f;
};
