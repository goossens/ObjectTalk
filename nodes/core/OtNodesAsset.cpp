//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//


#include "OtException.h"
#include "OtLog.h"

#include "OtNodesAsset.h"


//
//	OtNodesAsset::load
//

OtAssetBase::AssetState OtNodesAsset::load() {
	try {
		// load the nodes
		nodes.load(path);
		return readyState;

	} catch (const OtException& exception) {
		OtLogWarning("Can't load nodes [{}]: {}", path, exception.what());
		return invalidState;
	}
}


//
//	OtNodesAsset::save
//

OtAssetBase::AssetState OtNodesAsset::save() {
	try {
		// save the nodes
		nodes.save(path);
		return readyState;

	} catch (const OtException& exception) {
		OtLogWarning("Can't save nodes [{}]: {}", path, exception.what());
		return invalidState;
	}
}
