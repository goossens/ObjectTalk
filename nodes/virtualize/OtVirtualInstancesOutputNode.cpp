//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <string>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtInstancesAsset.h"

#include "OtNodesFactory.h"


//
//	OtVirtualInstancesOutputNode
//

class OtVirtualInstancesOutputNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", instances);
		addInputPin("Name", name);
	}

	// synchronize the asset with the incoming instances
	void onExecute() override {
		if (instances.isValid() && name.size()) {
			asset = "virtual:" + name;
			asset->setInstances(instances);

		} else if (name.size()) {
			asset = "virtual:" + name;
			asset->clearInstances();

		} else {
			asset.clear();
		}
	}

	static constexpr const char* nodeName = "Save Instances To Virtual";
	static constexpr int nodeCategory = OtNodeClass::virtualize;
	static constexpr int nodeKind = OtNodeClass::fixed;

protected:
	OtInstances instances;
	std::string name;
	OtAsset<OtInstancesAsset> asset;
};

static OtNodesFactoryRegister<OtVirtualInstancesOutputNode> type;
