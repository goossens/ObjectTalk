//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>

#include "nlohmann/json_fwd.hpp"

#include "OtAsset.h"
#include "OtTextureAsset.h"

#include "OtMaterial.h"


//
//	OtMaterial
//

class OtTerrainMaterialClass;
using OtTerrainMaterial = OtObjectPointer<OtTerrainMaterialClass>;

class OtTerrainMaterialClass : public OtMaterialClass {
public:
	// GUI to change material properties
	bool renderGUI() override;

	// (de)serialize material
	nlohmann::json serialize(std::filesystem::path* basedir) override;
	void deserialize(nlohmann::json data, std::filesystem::path* basedir) override;

	// get type name of material
	const char* getTypeName() override { return name; }

	// material name
	static constexpr char const* name = "Terrain";

	// get type definition
	static OtType getMeta();

private:
	// the scene renderer need access to our properties
	friend class OtSceneRenderer;

	// stored properties
	glm::vec3 region1Color = glm::vec3(0.965f, 0.894f, 0.678f);
	glm::vec3 region2Color = glm::vec3(0.494f, 0.784f, 0.314f);
	glm::vec3 region3Color = glm::vec3(0.584f, 0.553f, 0.522f);
	glm::vec3 region4Color = glm::vec3(0.9f, 0.9f, 0.9f);

	OtAsset<OtTextureAsset> region1Texture;
	OtAsset<OtTextureAsset> region2Texture;
	OtAsset<OtTextureAsset> region3Texture;
	OtAsset<OtTextureAsset> region4Texture;

	float region1Transition = 0.25f;
	float region2Transition = 0.5f;
	float region3Transition = 0.75f;

	float region1Overlap = 0.05f;
	float region2Overlap = 0.05f;
	float region3Overlap = 0.05f;

	float scale = 1.0f;
};
