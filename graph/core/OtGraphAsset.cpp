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

#include "OtAssetFactory.h"
#include "OtGraphAsset.h"

#include "OtImageFilterNodes.h"
#include "OtImageGeneratorNodes.h"
#include "OtInputNodes.h"
#include "OtMathNodes.h"
#include "OtOutputNodes.h"
#include "OtProbeNodes.h"


//
//	Register texture types
//

static OtAssetFactoryRegister<OtGraphAsset> otgRegistration{".otg"};


//
//
//

OtGraphAsset::OtGraphAsset() {
	// create the graph
	graph = std::make_unique<OtGraph>();
	OtImageFilterNodesRegister(*graph);
	OtImageGeneratorNodesRegister(*graph);
	OtInputNodesRegister(*graph);
	OtMathNodesRegister(*graph);
	OtOutputNodesRegister(*graph);
	OtProbeNodesRegister(*graph);
}


//
//	OtGraphAsset::load
//

bool OtGraphAsset::load() {
	try {
		// load the graph
		graph->load(path);
		return true;

	} catch (const OtException& exception) {
		OtLogWarning(OtFormat("Can't load graph [%s]: %s", path.c_str(), exception.what()));
		return false;
	}
}


//
//	OtGraphAsset::save
//

bool OtGraphAsset::save() {
	try {
		// save the graph
		graph->save(path);
		return true;

	} catch (const OtException& exception) {
		OtLogWarning(OtFormat("Can't save graph [%s]: %s", path.c_str(), exception.what()));
		return false;
	}
}
