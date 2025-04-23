//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <vector>

#include "OtAsset.h"
#include "OtAssetBase.h"

#include "OtModel.h"

//
//	OtModelAsset
//

class OtModelAsset : public OtAssetBase {
public:
	// access the model
	inline OtModel& getModel() { return model; }

	// asset properties
	static constexpr bool canHandleVirtual = false;
	static constexpr const char* supportedFileTypes = ".fbx,.glb,.gltf,.json,.md5mesh,.obj";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the model
	OtAssetBase::State load() override;

private:
	// the actual model
	OtModel model;
};
