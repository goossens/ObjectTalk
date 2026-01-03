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
//	OtCirclePrimitive
//

class OtCirclePrimitive : public OtPrimitiveBase {
public:
	// access properties
	inline void setSegments(int s) { segments = s; }
	inline void setThetaStart(float ts) { thetaStart = ts; }
	inline void setThetaLength(float tl) { thetaLength = tl; }

	inline int getSegments() { return segments; }
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
	static constexpr char const* name = "Circle";

private:
	// generate mesh
	void createMesh(OtMesh* mesh) override;

	// geometry
	int segments = 32;
	float thetaStart = 0.0f;
	float thetaLength = 360.0f;
};
