//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtAsset.h"
#include "OtAssetBase.h"
#include "OtGeometry.h"


//
//	OtGeometryAsset
//

class OtGeometryAsset : public OtAssetBase {
public:
	// access the geometry
	inline OtGeometry& getGeometry() { return geometry; }
	inline void setGeometry(OtGeometry& g) { geometry = g; state = State::ready; }
	inline void clearGeometry() { geometry.clear(); state = State::missing; }

	// asset properties
	static constexpr bool canHandleVirtual = true;
	static constexpr const char* supportedFileTypes = ".blend,.fbx,.glb,.gltf,.obj";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the asset
	OtAssetBase::State load() override;

private:
	// the actual geometry
	OtGeometry geometry;
};
