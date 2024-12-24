//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "glm/glm.hpp"
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtNodesFactory.h"


//
//	OtUnaryFloatNode
//

class OtUnaryFloatNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("A", a);
		addOutputPin("Result", result);
	}

	// render custom fields
	void customRendering(float width) override {
		ImGui::SetNextItemWidth(width);
		auto old = serialize().dump();

		if (OtUi::selectorEnum("##operator", &op, operators, operatorCount)) {
			oldState = old;
			newState = serialize().dump();
			needsEvaluating = true;
			needsSaving = true;
		}
	}

	float getCustomRenderingWidth() override {
		return 100.0f;
	}

	float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize node
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["operator"] = op;
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		op = data->value("operator", Operator::neg);
	}

	// compare values
	void onExecute() override {
		float b;

		switch (op) {
			case Operator::neg: result = -a; break;

			case Operator::abs: result = std::abs(a); break;
			case Operator::round: result = std::round(a); break;
			case Operator::floor: result = std::floor(a); break;
			case Operator::ceil: result = std::ceil(a); break;
			case Operator::truncate: result = std::trunc(a); break;
			case Operator::fraction: result = std::modf(a, &b); break;
			case Operator::sign: result = (x > 0.0f) ? 1.0f : ((x < 0.0f) ? -1.0f : 0.0f); break;

			case Operator::sqrt: result = std::sqrt(a); break;
			case Operator::invSqrt: result = 1.0f / std::sqrt(a); break;

			case Operator::sin: result = std::sin(a); break;
			case Operator::cos: result = std::cos(a); break;
			case Operator::tan: result = std::tan(a); break;
			case Operator::asin: result = std::asin(a); break;
			case Operator::acos: result = std::acos(a); break;
			case Operator::atan: result = std::atan(a); break;

			case Operator::radians: result = glm::radians(a); break;
			case Operator::degrees: result = glm::degrees(a); break;

			case Operator::log: result = std::log(a); break;
			case Operator::exp: result = std::exp(a); break;
		}
	}

	static constexpr const char* nodeName = "Unary Float";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::math;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::flexible;

protected:
	enum class Operator {
		neg,
		abs,
		round,
		floor,
		ceil,
		truncate,
		fraction,
		sign,
		sqrt,
		invSqrt,
		sin,
		cos,
		tan,
		asin,
		acos,
		atan,
		radians,
		degrees,
		log,
		exp
	};

	static constexpr const char* operators[] = {
		"Neg",
		"Abs",
		"Round",
		"Floor",
		"Ceil",
		"Truncate",
		"Fraction",
		"Sign",
		"Sqrt",
		"InvSqrt",
		"Sin",
		"Cos",
		"Tan",
		"Asin",
		"Acos",
		"Atan",
		"Radians",
		"Degrees",
		"Log",
		"Exp"
	};

	static constexpr size_t operatorCount = sizeof(operators) / sizeof(*operators);

	Operator op = Operator::neg;
	float a = 0.0f;
	float result = 0.0f;
};

static OtNodesFactoryRegister<OtUnaryFloatNode> type;
