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

#include "OtGeometry.h"


//
//	OtBoxGeometryClass
//

class OtBoxGeometryClass;
using OtBoxGeometry = OtObjectPointer<OtBoxGeometryClass>;

class OtBoxGeometryClass : public OtGeometryClass {
public:
	// initialize geometry
	void init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setWidthSegments(int widthSegments);
	OtObject setHeightSegments(int heightSegments);
	OtObject setDepthSegments(int depthSegments);

	// UI to change geometry properties
	bool renderUI() override;

	// (de)serialize geometry
	nlohmann::json serialize(std::string* basedir) override;
	void deserialize(nlohmann::json data, std::string* basedir) override;

	// get type name of geometry
	const char* getTypeName() override { return name; }

	// geometry name
	static constexpr char const* name = "Box";

	// get type definition
	static OtType getMeta();

private:
	// generate geometry
	void fillGeometry() override;

	// create one side of the BoxGeometry
	void buildPlane(int udir, int vdir, float w, float h, float d, int gridX, int gridY, glm::vec3 (*cb)(float, float, float));

	// attributes
	int widthSegments = 1;
	int heightSegments = 1;
	int depthSegments = 1;
};
