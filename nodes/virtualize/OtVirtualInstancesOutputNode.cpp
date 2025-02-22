//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
		if (!asset.isNull()) {
			asset->clearInstances();
		}

		if (name.size()) {
			asset = "virtual:" + name;

			if (instances.isValid()) {
				asset->setInstances(instances);
			}

		} else {
			asset.clear();
		}
	}

	static constexpr const char* nodeName = "Save Instances To Virtual";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::virtualize;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	OtInstances instances;
	std::string name;
	OtAsset<OtInstancesAsset> asset;
};

static OtNodesFactoryRegister<OtVirtualInstancesOutputNode> registration;
