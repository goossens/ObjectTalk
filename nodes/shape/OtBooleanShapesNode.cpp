//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtShape.h"
#include "OtUi.h"

#include "OtNodesFactory.h"


//
//	OtBooleanShapesNode
//

class OtBooleanShapesNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("A", a);
		addInputPin("B", b);
		addOutputPin("Result", result);
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		ImGui::SetNextItemWidth(itemWidth);
		return OtUi::selectorEnum("##operator", &op, operators, operatorCount);
	}

	inline float getCustomRenderingWidth() override {
		return OtUi::size(5.0f);
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["operator"] = op;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		op = data->value("operator", Operator::intersect);
	}

	// compare values
	inline void onExecute() override {
		if (a.isValid() && b.isValid()) {
			switch (op) {
				case Operator::intersect: result = a & b; break;
				case Operator::union_: result = a | b; break;
				case Operator::difference: result = a - b; break;
				case Operator::xor_: result = a ^ b; break;
			}

		} else {
			result.clear();
		}
	}

	static constexpr const char* nodeName = "Boolean Shapes";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::shape;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	enum class Operator {
		intersect,
		union_,
		difference,
		xor_
	};

	static constexpr const char* operators[] = {
		"Intersect",
		"Union",
		"Difference",
		"Xor"
	};

	static constexpr size_t operatorCount = sizeof(operators) / sizeof(*operators);

	Operator op = Operator::intersect;
	OtShape a;
	OtShape b;
	OtShape result;
};

static OtNodesFactoryRegister<OtBooleanShapesNode> registration;
