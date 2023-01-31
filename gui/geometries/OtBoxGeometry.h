//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
typedef std::shared_ptr<OtBoxGeometryClass> OtBoxGeometry;

class OtBoxGeometryClass : public OtGeometryClass {
public:
	// initialize geometry
	void init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setWidth(float width);
	OtObject setHeight(float height);
	OtObject setDepth(float depth);
	OtObject setWidthSegments(int widthSegments);
	OtObject setHeightSegments(int heightSegments);
	OtObject setDepthSegments(int depthSegments);

	// GUI to change geometry properties
	bool renderGUI() override;

	// (de)serialize geometry
	nlohmann::json serialize() override;
	void deserialize(nlohmann::json data) override;

	// get type name of geometry
	const char* getTypeName() override { return name; }

	// geometry name
	static constexpr char const* name = "Box";

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtBoxGeometry create();

private:
	// generate geometry
	void fillGeometry() override;

	// create one side of the BoxGeometry
	void buildPlane(int udir, int vdir, float w, float h, float d, int gridX, int gridY, glm::vec3 (*cb)(float, float, float));

	// attributes
	float width = 1.0;
	float height = 1.0;
	float depth = 1.0;
	int widthSegments = 1;
	int heightSegments = 1;
	int depthSegments = 1;
};
