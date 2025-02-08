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
#include "OtShape.h"


//
//	OtShapeAsset
//

class OtShapeAsset : public OtAssetBase {
public:
	// access the shape
	inline OtShape& getShape() { return shape; }
	inline void setShape(OtShape& s) { shape = s; state = State::ready; }
	inline void clearShape() { shape.clear(); state = State::missing; }

	// asset properties
	static constexpr bool canHandleVirtual = true;
	static constexpr const char* supportedFileTypes = ".shape";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the asset
	OtAssetBase::State load() override;

private:
	// the actual shape
	OtShape shape;
};
