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

#include "OtTexture.h"

#include "OtMaterial.h"


//
//	OtMaterial
//

class OtBlendMapMaterialClass;
using OtBlendMapMaterial = OtObjectPointer<OtBlendMapMaterialClass>;

class OtBlendMapMaterialClass : public OtMaterialClass {
public:
	// update material
	void update() override;

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
	std::filesystem::path blendMapPath;

	std::filesystem::path nonePath;
	std::filesystem::path redPath;
	std::filesystem::path greenPath;
	std::filesystem::path bluePath;

	std::filesystem::path normalsNonePath;
	std::filesystem::path normalsRedPath;
	std::filesystem::path normalsGreenPath;
	std::filesystem::path normalsBluePath;

	float metallic = 0.5f;
	float roughness = 0.5f;
	float emissive = 0.0f;
	float scale = 1.0f;

	// runtime properties
	OtTexture blendMapTexture;

	OtTexture noneTexture;
	OtTexture redTexture;
	OtTexture greenTexture;
	OtTexture blueTexture;

	OtTexture normalsNoneTexture;
	OtTexture normalsRedTexture;
	OtTexture normalsGreenTexture;
	OtTexture normalsBlueTexture;

	bool updateBlendMapTexture = false;

	bool updateNoneTexture = false;
	bool updateRedTexture = false;
	bool updateGreenTexture = false;
	bool updateBlueTexture = false;

	bool updateNormalsNoneTexture = false;
	bool updateNormalsRedTexture = false;
	bool updateNormalsGreenTexture = false;
	bool updateNormalsBlueTexture = false;
};
