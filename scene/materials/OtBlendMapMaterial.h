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

class OtBlendMapMaterialClass;
using OtBlendMapMaterial = OtObjectPointer<OtBlendMapMaterialClass>;

class OtBlendMapMaterialClass : public OtMaterialClass {
public:
	// GUI to change geometry properties
	bool renderGUI() override;

	// (de)serialize geometry
	nlohmann::json serialize(std::filesystem::path* basedir) override;
	void deserialize(nlohmann::json data, std::filesystem::path* basedir) override;

	// get type name of geometry
	const char* getTypeName() override { return name; }

	// material name
	static constexpr char const* name = "BlendMap";

	// get type definition
	static OtType getMeta();

private:
	// the scene renderer need access to our properties
	friend class OtSceneRenderer;

	// stored properties
	OtAsset<OtTextureAsset> blendMapTexture;

	OtAsset<OtTextureAsset> noneTexture;
	OtAsset<OtTextureAsset> redTexture;
	OtAsset<OtTextureAsset> greenTexture;
	OtAsset<OtTextureAsset> blueTexture;

	OtAsset<OtTextureAsset> noneNormalsTexture;
	OtAsset<OtTextureAsset> redNormalsTexture;
	OtAsset<OtTextureAsset> greenNormalsTexture;
	OtAsset<OtTextureAsset> blueNormalsTexture;

	float metallic = 0.5f;
	float roughness = 0.5f;
	float scale = 1.0f;

	// runtime properties
};
