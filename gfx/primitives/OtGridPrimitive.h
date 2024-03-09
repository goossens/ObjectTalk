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
//	OtGridPrimitive
//

class OtGridPrimitive : public OtPrimitiveBase {
public:
	// acces properties
	inline void setWidthSegments(int ws) { widthSegments = ws; }
	inline void setDepthSegments(int ds) { depthSegments = ds; }

	inline int getWidthSegments() { return widthSegments; }
	inline int getDepthSegments() { return depthSegments; }

	// UI to change primitive's properties
	bool renderUI() override;

	// (de)serialize primitive
	nlohmann::json serialize(std::string* basedir) override;
	void deserialize(nlohmann::json data, std::string* basedir) override;

	// get type name of primitive
	const char* getTypeName() override { return name; }

	// primitive name
	static constexpr char const* name = "Grid";

private:
	// generate mesh
	void createMesh(OtMesh* mesh) override;

	// properties
	int widthSegments = 1;
	int depthSegments = 1;
};
