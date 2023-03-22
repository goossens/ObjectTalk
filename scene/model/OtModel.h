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
#include <vector>

#include "OtModelMaterial.h"
#include "OtModelMesh.h"

//
//	OtModel
//

class OtModel {
public:
	// clear the model
	void clear();

	// load the model
	void load(const std::filesystem::path& path);

	// see if we have a valid model
	bool isValid() { return valid; }

private:
	// the renderer accesses our properties
	friend class OtSceneRenderer;

	// path to main model file
	std::filesystem::path modelPath;

	// our meshes and materials
	std::vector<OtModelMesh> meshes;
	std::vector<OtModelMaterial> materials;

	// are we fully loaded and valid
	bool valid = false;
};
