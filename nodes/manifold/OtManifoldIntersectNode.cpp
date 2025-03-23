//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtManifold.h"

#include "OtNodesFactory.h"


//
//	OtManifoldIntersectNode
//

class OtManifoldIntersectNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("A", a);
		addInputPin("B", b);
		addOutputPin("Result", result);
	}

	// create the manifold
	inline void onExecute() override {
		if (a.isValid() && b.isValid()) {
			result = a.intersectManifolds(b);

		} else {
			result.clear();
		}
	}

	static constexpr const char* nodeName = "Manifold Intersect";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::manifold;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	OtManifold a;
	OtManifold b;
	OtManifold result;
};

static OtNodesFactoryRegister<OtManifoldIntersectNode> registration;
