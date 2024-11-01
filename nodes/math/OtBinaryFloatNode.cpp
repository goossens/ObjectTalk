//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
		return fieldWidth;
	}

	float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize node
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["operator"] = op;
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		op = data->value("operator", addOperator);
	}

	// compare values
	void onExecute() override {
		switch (op) {
			case addOperator: result = a + b; break;
			case subtractOperator: result = a - b; break;
			case multiplyOperator: result = a * b; break;
			case divideOperator: result =  a / (b == 0.0f ? 0.000001 : b); break;
			case moduloOperator: result = std::fmod(a, b); break;
			case remainderOperator: result = std::remainder(a, b); break;
			case powOperator: result = std::pow(a, b); break;
			case minOperator: result = std::min(a, b); break;
			case maxOperator: result = std::max(a, b); break;
		}
	}

	static constexpr const char* nodeName = "Binary Float";
	static constexpr int nodeCategory = OtNodeClass::math;
	static constexpr int nodeKind = OtNodeClass::flexible;
	static constexpr float fieldWidth = 100.0f;

protected:
	enum {
		addOperator,
		subtractOperator,
		multiplyOperator,
		divideOperator,
		moduloOperator,
		remainderOperator,
		powOperator,
		minOperator,
		maxOperator
	};

	static constexpr const char* operatorTypes[] = {
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

	static constexpr size_t operatorTypesCount = sizeof(operatorTypes) / sizeof(*operatorTypes);

	int op = addOperator;
	float a = 0.0f;
	float b = 0.0f;
	float result = 0.0f;
};

static OtNodesFactoryRegister<OtBinaryFloatNode> type;
