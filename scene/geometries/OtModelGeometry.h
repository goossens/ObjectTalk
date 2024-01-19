//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "nlohmann/json_fwd.hpp"

#include "OtGeometry.h"


//
//	OtModelGeometryClass
//

class OtModelGeometryClass;
using OtModelGeometry = OtObjectPointer<OtModelGeometryClass>;

class OtModelGeometryClass : public OtGeometryClass {
public:
	// initialize geometry
	void init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setModel(const std::string& name);
	OtObject setScale(float scale);

	// UI to change geometry properties
	bool renderUI() override;

	// (de)serialize geometry
	nlohmann::json serialize(std::string* basedir) override;
	void deserialize(nlohmann::json data, std::string* basedir) override;

	// get type name of geometry
	const char* getTypeName() override { return name; }

	// geometry name
	static constexpr char const* name = "Model";

	// get type definition
	static OtType getMeta();

private:
	// generate geometry
	void fillGeometry() override;

	// properties
	std::string modelPath;
};
