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
//	OtPlanePrimitive
//

class OtPlanePrimitive : public OtPrimitiveBase {
public:
	// acces properties
	inline void setWidthSegments(int ws) { widthSegments = ws; }
	inline void setHeightSegments(int hs) { heightSegments = hs; }

	inline int getWidthSegments() { return widthSegments; }
	inline int getHeightSegments() { return heightSegments; }

	// UI to change primitive's properties
	bool renderUI() override;

	// (de)serialize primitive
	nlohmann::json serialize(std::string* basedir) override;
	void deserialize(nlohmann::json data, std::string* basedir) override;

	// get type name of primitive
	const char* getTypeName() override { return name; }

	// primitive name
	static constexpr char const* name = "Plane";

private:
	// generate mesh
	void createMesh(OtMesh* mesh) override;

	// properties
	int widthSegments = 1;
	int heightSegments = 1;
};
