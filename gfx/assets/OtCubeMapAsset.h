//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
	// access the cubemap
	inline OtCubeMap& getCubeMap() { return cubemap; }

	// asset properties
	static constexpr bool hasEditor = false;
	static constexpr bool canHandleVirtual = false;
	static constexpr const char* supportedFileTypes = ".dds,.hdr,.json,.ktx";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the asset
	OtAssetBase::State load() override;

private:
	// the actual cubemap
	OtCubeMap cubemap;

	// post-processing event handler
	uv_async_t* postProcessingEventHandle = nullptr;
};
