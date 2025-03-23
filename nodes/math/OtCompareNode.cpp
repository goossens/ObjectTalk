//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
		return 140.0f;
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, std::string* /* basedir */) override {
		(*data)["operator"] = op;
	}

	inline void customDeserialize(nlohmann::json* data, std::string* /* basedir */) override {
		op = data->value("operator", Operator::equal);
	}

	// compare values
	inline void onExecute() override {
		switch (op) {
			case Operator::equal: result = (a == b); break;
			case Operator::notEqual: result = (a != b); break;
			case Operator::less: result = (a < b); break;
			case Operator::lessEqual: result = (a <= b); break;
			case Operator::greater: result = (a > b); break;
			case Operator::greatertEqual: result = (a >= b); break;
		}
	}

	static constexpr const char* nodeName = "Compare";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::math;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::flexible;

protected:
	enum class Operator {
		equal,
		notEqual,
		less,
		lessEqual,
		greater,
		greatertEqual
	};

	static constexpr const char* operators[] = {
		"Equal",
		"Not Equal",
		"Less",
		"Less Equal",
		"Greater",
		"Greater Equal"
	};

	static constexpr size_t operatorCount = sizeof(operators) / sizeof(*operators);

	Operator op = Operator::equal;
	float a = 0.0f;
	float b = 0.0f;
	bool result = false;
};

static OtNodesFactoryRegister<OtCompareNode> registration;
