//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtException.h"

#include "OtAsset.h"
#include "OtAssetBase.h"
#include "OtHeightMap.h"


//
//	OtHeightMapAsset
//

class OtHeightMapAsset : public OtAssetBase {
public:
	// access the heightmap
	inline OtHeightMap& getHeightMap() { return heightMap; }
	inline void setHeightMap(OtHeightMap& g) { heightMap = g; state = State::ready; }
	inline void clearHeightMap() { heightMap.clear(); state = State::missing; }

	// asset properties
	static constexpr bool hasEditor = false;
	static constexpr bool canHandleVirtual = true;
	static constexpr const char* supportedFileTypes = ".tiff";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the asset
	OtAssetBase::State load() override {
		try {
			// try to load the heightmap
			heightMap.load(path);
			return State::ready;

		} catch (const OtException& exception) {
			errorMessage = exception.what();
			return State::invalid;
		}
	}

private:
	// the actual heightmap
	OtHeightMap heightMap;
};
