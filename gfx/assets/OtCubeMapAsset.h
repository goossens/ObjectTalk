//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtAsset.h"
#include "OtAssetBase.h"
#include "OtCubeMap.h"


//
//	OtCubeMapAsset
//

class OtCubeMapAsset : public OtAssetBase {
public:
	// load the asset
	bool load(const std::filesystem::path& path) override;

	// get the CubeMap handle
	inline OtCubeMap& getCubeMap() { return cubemap; }

private:
	// the actual cubemap
	OtCubeMap cubemap;
};
