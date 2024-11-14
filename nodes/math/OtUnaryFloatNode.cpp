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

		if (OtUi::selectorEnum("##operator", &op, operatorTypes, operatorTypesCount)) {
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
		op = data->value("operator", negOperator);
	}

	// compare values
	void onExecute() override {
		float b;

		switch (op) {
			case negOperator: result = -a; break;

			case absOperator: result = std::abs(a); break;
			case roundOperator: result = std::round(a); break;
			case floorOperator: result = std::floor(a); break;
			case ceilOperator: result = std::ceil(a); break;
			case truncateOperator: result = std::trunc(a); break;
			case fractionOperator: result = std::modf(a, &b); break;
			case signOperator: result = (x > 0.0f) ? 1.0f : ((x < 0.0f) ? -1.0f : 0.0f); break;

			case sqrtOperator: result = std::sqrt(a); break;
			case invSqrtOperator: result = 1.0f / std::sqrt(a); break;

			case sinOperator: result = std::sin(a); break;
			case cosOperator: result = std::cos(a); break;
			case tanOperator: result = std::tan(a); break;
			case asinOperator: result = std::asin(a); break;
			case acosOperator: result = std::acos(a); break;
			case atanOperator: result = std::atan(a); break;

			case radiansOperator: result = glm::radians(a); break;
			case degreesOperator: result = glm::degrees(a); break;

			case logOperator: result = std::log(a); break;
			case expOperator: result = std::exp(a); break;
		}
	}

	static constexpr const char* nodeName = "Unary Float";
	static constexpr int nodeCategory = OtNodeClass::math;
	static constexpr int nodeKind = OtNodeClass::flexible;

protected:
	enum {
		negOperator,
		absOperator,
		roundOperator,
		floorOperator,
		ceilOperator,
		truncateOperator,
		fractionOperator,
		signOperator,
		sqrtOperator,
		invSqrtOperator,
		sinOperator,
		cosOperator,
		tanOperator,
		asinOperator,
		acosOperator,
		atanOperator,
		radiansOperator,
		degreesOperator,
		logOperator,
		expOperator
	};

	static constexpr const char* operatorTypes[] = {
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

	static constexpr size_t operatorTypesCount = sizeof(operatorTypes) / sizeof(*operatorTypes);

	int op = negOperator;
	float a = 0.0f;
	float result = 0.0f;
};

static OtNodesFactoryRegister<OtUnaryFloatNode> type;
