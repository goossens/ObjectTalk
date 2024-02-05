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
	bool load() override;

	// access the CubeMap
	inline OtCubeMap& getCubeMap() { return cubemap; }
	inline void setCubeMap(OtCubeMap& c) { cubemap = c; }

	// asset properties
	static constexpr bool canHandleVirtual = false;
	static constexpr const char* supportedFileTypes = ".dds,.ktx,.cubemap,.hdr";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

private:
	// the actual cubemap
	OtCubeMap cubemap;
};
