//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//


#include "OtException.h"
#include "OtFormat.h"
#include "OtLog.h"

#include "OtGraphAsset.h"

#include "OtInputNodes.h"
#include "OtMathNodes.h"
#include "OtOutputNodes.h"
#include "OtProbeNodes.h"
#include "OtTextureFilterNodes.h"
#include "OtTextureGeneratorNodes.h"


//
//
//

OtGraphAsset::OtGraphAsset() {
	// create the graph
	graph = std::make_unique<OtGraph>();
	OtInputNodesRegister(*graph);
	OtMathNodesRegister(*graph);
	OtOutputNodesRegister(*graph);
	OtProbeNodesRegister(*graph);
	OtTextureFilterNodesRegister(*graph);
	OtTextureGeneratorNodesRegister(*graph);
}


//
//	OtGraphAsset::load
//

OtAssetBase::AssetState OtGraphAsset::load() {
	try {
		// load the graph
		graph->load(path);
		return readyState;

	} catch (const OtException& exception) {
		OtLogWarning(OtFormat("Can't load graph [%s]: %s", path.c_str(), exception.what()));
		return invalidState;
	}
}


//
//	OtGraphAsset::save
//

OtAssetBase::AssetState OtGraphAsset::save() {
	try {
		// save the graph
		graph->save(path);
		return readyState;

	} catch (const OtException& exception) {
		OtLogWarning(OtFormat("Can't save graph [%s]: %s", path.c_str(), exception.what()));
		return invalidState;
	}
}
