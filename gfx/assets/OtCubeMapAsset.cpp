//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
#include "OtCubeMapAsset.h"


//
//	Register CubeMap types
//

static OtAssetFactoryRegister<OtCubeMapAsset> ddsRegistration{".dds"};
static OtAssetFactoryRegister<OtCubeMapAsset> ktxRegistration{".ktx"};
static OtAssetFactoryRegister<OtCubeMapAsset> cubemapRegistration{".cubemap"};


//
//	OtCubeMapAsset::load
//

bool OtCubeMapAsset::load(const std::filesystem::path &path) {
	try {
		// try to load the cubemap
		cubemap.load(path);
		return true;

	} catch (const OtException& exception) {
		OtLogWarning(OtFormat("Can't load CubeMap [%s]: %s", path.c_str(), exception.what()));
		return false;
	}
}
