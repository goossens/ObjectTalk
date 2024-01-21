//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>

#include "OtAssetBase.h"
#include "OtGraph.h"


//
//	OtGraphAsset
//

class OtGraphAsset : public OtAssetBase {
public:
	// load/save the graph
	bool load(const std::string& path) override;
	bool save() override;

	// access the graph
	inline OtGraph* getGraph() { return graph.get(); }

private:
	// the actual graph
	std::unique_ptr<OtGraph> graph;
};
