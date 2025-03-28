//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtNodesFactory.h"


//
//	OtBinaryFloatNode
//

class OtBinaryFloatNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("A", a);
		addInputPin("B", b);
		addOutputPin("Result", result);
	}

	// render custom fields
	inline void customRendering(float itemWidth) override {
		ImGui::SetNextItemWidth(itemWidth);
		auto old = serialize().dump();

		if (OtUi::selectorEnum("##operator", &op, operators, operatorCount)) {
			oldState = old;
			newState = serialize().dump();
			needsEvaluating = true;
			needsSaving = true;
		}
	}

	inline float getCustomRenderingWidth() override {
		return 100.0f;
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, std::string* /* basedir */) override {
		(*data)["operator"] = op;
	}

	inline void customDeserialize(nlohmann::json* data, std::string* /* basedir */) override {
		op = data->value("operator", Operator::add);
	}

	// compare values
	inline void onExecute() override {
		switch (op) {
			case Operator::add: result = a + b; break;
			case Operator::subtract: result = a - b; break;
			case Operator::multiply: result = a * b; break;
			case Operator::divide: result =  a / b; break;
			case Operator::modulo: result = std::fmod(a, b); break;
			case Operator::remainder: result = std::remainder(a, b); break;
			case Operator::pow: result = std::pow(a, b); break;
			case Operator::min: result = std::min(a, b); break;
			case Operator::max: result = std::max(a, b); break;
		}
	}

	static constexpr const char* nodeName = "Binary Float";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::math;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::flexible;

protected:
	enum class Operator {
		add,
		subtract,
		multiply,
		divide,
		modulo,
		remainder,
		pow,
		min,
		max
	};

	static constexpr const char* operators[] = {
		"Add",
		"Subtract",
		"Multiply",
		"Divide",
		"Modulo",
		"Remainder",
		"Pow",
		"Min",
		"Max"
	};

	static constexpr size_t operatorCount = sizeof(operators) / sizeof(*operators);

	Operator op = Operator::add;
	float a = 0.0f;
	float b = 0.0f;
	float result = 0.0f;
};

static OtNodesFactoryRegister<OtBinaryFloatNode> registration;
