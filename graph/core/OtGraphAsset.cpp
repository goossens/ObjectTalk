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


//
//	Register texture types
//

static OtAssetFactoryRegister<OtGraphAsset> otgRegistration{".otg"};


//
//	OtGraphAsset::load
//

bool OtGraphAsset::load(const std::string& path) {
	try {
		// create and load the graph
		graph = std::make_unique<OtGraph>();
		graph->load(path);
		return true;

	} catch (const OtException& exception) {
		OtLogWarning(OtFormat("Can't load graph [%s]: %s", path.c_str(), exception.what()));
		return false;
	}
}
