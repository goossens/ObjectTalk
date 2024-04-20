//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtNodesFactory.h"


//
//	OtCompareNode
//

class OtCompareNode : public OtNodeClass {
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

		if (OtUiSelectorEnum("##operator", &op, operatorTypes, operatorTypesCount)) {
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
		op = data->value("operator", equalOperator);
	}

	// compare values
	void onExecute() override {
		switch (op) {
			case equalOperator: result = (a == b); break;
			case notEqualOperator: result = (a != b); break;
			case lessOperator: result = (a < b); break;
			case lessEqualOperator: result = (a <= b); break;
			case greaterOperator: result = (a > b); break;
			case greatertEqualOperator: result = (a >= b); break;
		}
	}

	static constexpr const char* nodeName = "Compare";
	static constexpr int nodeCategory = OtNodeClass::math;
	static constexpr int nodeKind = OtNodeClass::flexible;
	static constexpr float fieldWidth = 140.0f;

protected:
	enum {
		equalOperator,
		notEqualOperator,
		lessOperator,
		lessEqualOperator,
		greaterOperator,
		greatertEqualOperator
	};

	static constexpr const char* operatorTypes[] = {
		"Equal",
		"Not Equal",
		"Less",
		"Less Equal",
		"Greater",
		"Greater Equal"
	};

	static constexpr size_t operatorTypesCount = sizeof(operatorTypes) / sizeof(*operatorTypes);

	int op = equalOperator;
	float a = 0.0f;
	float b = 0.0f;
	bool result = false;
};

static OtNodesFactoryRegister<OtCompareNode> type;
