//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtNodesFactory.h"


//
//	OtSeparateXyNode
//

class OtSeparateXyNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Value", value);
		addOutputPin("X", x);
		addOutputPin("Y", y);
	}

	// separate values
	void onExecute() override {
		x = value.x;
		y = value.y;
	}

	static constexpr const char* nodeName = "Separate XY";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::transform;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::flexible;

protected:
	glm::vec2 value{0.0f};
	float x{0.0f};
	float y{0.0f};
};

static OtNodesFactoryRegister<OtSeparateXyNode> type;
