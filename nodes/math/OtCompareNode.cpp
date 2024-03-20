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

		if (OtUiSelectorEnum("##compareOperator", compareOperator, operandTypes, operandTypesCount)) {
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
		(*data)["compareOperator"] = compareOperator;
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		compareOperator = data->value("compareOperator", equal);
	}

	// compare values
	void onExecute() override {
		switch (compareOperator) {
			case equal: result = (a == b); break;
			case notEqual: result = (a != b); break;
			case less: result = (a < b); break;
			case lessEqual: result = (a <= b); break;
			case greater: result = (a > b); break;
			case greatertEqual: result = (a >= b); break;
		}
	}

	static constexpr const char* nodeName = "Compare";
	static constexpr int nodeCategory = OtNodeClass::math;
	static constexpr int nodeKind = OtNodeClass::flexible;
	static constexpr float fieldWidth = 140.0f;

protected:
	enum {
		equal,
		notEqual,
		less,
		lessEqual,
		greater,
		greatertEqual
	};

	static constexpr const char* operandTypes[] = {
		"Equal",
		"Not Equal",
		"Less",
		"Less Equal",
		"Greater",
		"Greater Equal"
	};

	static constexpr size_t operandTypesCount = sizeof(operandTypes) / sizeof(*operandTypes);

	int compareOperator = equal;
	float a = 0.0f;
	float b = 0.0f;
	bool result = false;
};

static OtNodesFactoryRegister<OtCompareNode> type;
