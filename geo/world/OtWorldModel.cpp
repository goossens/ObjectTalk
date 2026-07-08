//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtWorldModel.h"

#include "OtWorldTile.h"
#include "OtWorldTileCache.h"
#include "OtWorldTileID.h"


/*
	world size:		2048 x 2048 meters
	quad size:		128
	tile size:		132
	border size:	2

	level	spacing		tiles
	0		8			4
	1		4			16
	2		2			64
	3		1			256
*/


//
//	OtWorldModel::generate
//

void OtWorldModel::generate() {
}
