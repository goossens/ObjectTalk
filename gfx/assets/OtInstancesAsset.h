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
#include "OtInstances.h"


//
//	OtInstancesAsset
//

class OtInstancesAsset : public OtAssetBase {
public:
	// access the instances
	inline OtInstances& getInstances() { return instances; }
	inline void setInstances(OtInstances& i) { instances = i; markReady(); }
	inline void clearInstances() { instances.clear(); markMissing(); }

	// asset properties
	static constexpr bool canHandleVirtual = true;
	static constexpr const char* supportedFileTypes = ".oti";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the asset
	AssetState load() override;

private:
	// the actual instances
	OtInstances instances;
};
