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
//	OtCylinderPrimitive
//

class OtCylinderPrimitive : public OtPrimitiveBase {
public:
	// access properties
	inline void setTopRadius(float tr) { topRadius = tr; }
	inline void setBottomRadius(float br) { bottomRadius = br; }
	inline void setRadialSegments(int rs) { radialSegments = rs; }
	inline void setHeightSegments(int hs) { heightSegments = hs; }
	inline void setOpenEnded(bool oe) { openEnded = oe; }
	inline void setThetaStart(int ts) { thetaStart = ts; }
	inline void setThetaLength(int tl) { thetaLength = tl; }

	inline float getTopRadius() { return topRadius; }
	inline float getBottomRadius() { return bottomRadius; }
	inline int getRadialSegments() { return radialSegments; }
	inline int getHeightSegments() { return heightSegments; }
	inline bool getOpenEnded() { return openEnded; }
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
	static constexpr char const* name = "Cylinder";

private:
	// generate mesh
	void createMesh(OtMesh* mesh) override;

	// generate parts
	void generateTorso(OtMesh* mesh);
	void generateCap(OtMesh* mesh, bool top);

	// geometry
	float topRadius = 1.0f;
	float bottomRadius = 1.0f;
	int radialSegments = 32;
	int heightSegments = 1;
	bool openEnded = false;
	float thetaStart = 0.0f;
	float thetaLength = 360.0f;
};
