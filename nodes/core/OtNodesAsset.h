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
#include "OtNodes.h"


//
//	OtNodesAsset
//

class OtNodesAsset : public OtAssetBase {
public:
	// access the nodes
	inline OtNodes& getNodes() { return nodes; }

	// asset properties
	static constexpr bool hasEditor = true;
	static constexpr bool canHandleVirtual = false;
	static constexpr const char* supportedFileTypes = ".otn";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load/save the nodes
	AssetState load() override;
	AssetState save() override;

private:
	// the actual nodes
	OtNodes nodes;
};
