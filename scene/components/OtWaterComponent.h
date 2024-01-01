//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"

#include "OtTextureAsset.h"


//
//	OtWaterComponent
//

class OtWaterComponent {
public:
	// GUI to change component properties
	bool renderGUI();

	// (de)serialize component
	nlohmann::json serialize(std::filesystem::path* basedir);
	void deserialize(nlohmann::json data, std::filesystem::path* basedir);

	// component name
	static constexpr char const* name = "Water";

	// stored properties
	float level = 0.0f;
	float distance = 100.0f;
	bool useRefractance = true;
	glm::vec3 color{0.5f, 0.6f, 0.8f};
	OtAsset<OtTextureAsset> normals;
	float scale = 10.0f;
	float speed = 1.0f;
	float metallic = 0.3f;
	float roughness = 0.2f;
	float ao = 0.25f;
	float reflectivity = 1.0f;

#if OT_DEBUG
	uint16_t reflectionTextureIndex = OtTexture::invalidIndex;
	uint16_t refractionTextureIndex = OtTexture::invalidIndex;
#endif
};
