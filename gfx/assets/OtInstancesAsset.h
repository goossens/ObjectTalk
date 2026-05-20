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
#include "OtInstances.h"


//
//	OtInstancesAsset
//

class OtInstancesAsset : public OtAssetBase {
public:
	// access the instances
	inline OtInstances& getInstances() { return instances; }
	inline void setInstances(OtInstances& i) { instances = i; state = State::ready; }
	inline void clearInstances() { instances.clear(); state = State::missing; }

	// asset properties
	static constexpr bool hasEditor = false;
	static constexpr bool canHandleVirtual = true;
	static constexpr const char* supportedFileTypes = ".oti";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the asset
	OtAssetBase::State load() override {
		try {
			// try to load the instances
			instances.load(path);
			return State::ready;

		} catch (const OtException& exception) {
			errorMessage = exception.what();
			return State::invalid;
		}
	}

private:
	// the actual instances
	OtInstances instances;
};
