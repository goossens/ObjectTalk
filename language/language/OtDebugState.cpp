//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtArray.h"
#include "OtDict.h"
#include "OtDebugState.h"
#include "OtSet.h"
#include "OtText.h"
#include "OtVM.h"


//
//	OtDebugState::update
//

void OtDebugState::update() {
	auto stack = OtVM::getStack();
	auto stackFrameCount = stack->getFrameCount();
	// process all stackframes
	for (size_t i = 0; i < stackFrameCount; i++) {
		auto& stackframe = stack->getFrame(i);
		auto bytecode = stackframe.bytecode;
		auto pc = *stackframe.pc;
		auto sp = OtVM::getStack()->raw();
		auto& frame = frames.emplace_back(bytecode->getModule(), bytecode->getLineNumber(pc));

		// process all variables in stackframe
		auto symbols = bytecode->getUsedSymbols(pc);

		for (auto& symbol : symbols) {
			auto name = OtIdentifier::name(symbol.id);

			switch (symbol.type) {
				case OtSymbol::Type::heap:
				addObject(frame.variables, name, symbol.object->get(symbol.id));
					break;

				case OtSymbol::Type::stack:
				addObject(frame.variables, name, sp[stackframe.offset + symbol.slot]);
					break;

				case OtSymbol::Type::capture:
					if (i == stackFrameCount - 1) {
						addObject(frame.variables, name, stack->getClosure()->get(symbol.id));
					}

					break;
			}
		}

		// sort the variables
		std::sort(frame.variables.begin(), frame.variables.end(), [](const Variable& a, const Variable& b) {
			return OtText::caseCmp(a.name, b.name) < 0;
		});
	}
}


//
//	OtDebugState::serialize
//

std::string OtDebugState::serialize() {
	auto frameList = nlohmann::json::array();

	for (auto& frame : frames) {
		auto variableList = nlohmann::json::array();

		for (auto& variable : frame.variables) {
			variableList.push_back(serializeVariable(variable));
		}

		auto data = nlohmann::json::object();
		data["module"] = frame.module;
		data["line"] = frame.line;
		data["variables"] = variableList;
		frameList.push_back(data);
	}

	return frameList.dump();
}


//
//	OtDebugState::deserialize
//

void OtDebugState::deserialize(const std::string& string) {
	frames.clear();
	auto data = nlohmann::json::parse(string);

	for (auto& frameData : data) {
		auto& frame = frames.emplace_back(frameData["module"], frameData["line"]);

		for (auto& variableData : frameData["variables"]) {
			deserializeVariable(frame.variables, variableData);
		}
	}
}


//
//	OtDebugState::addObject
//

void OtDebugState::addObject(std::vector<Variable>& list, const std::string_view& name, OtObject object) {
	auto& variable = list.emplace_back(name, object.getTypeName(), object->describe());

	if (object->hasMembers()) {
		object->eachMember([this, &variable](OtID id, OtObject memberObject) {
			addObject(variable.members, OtIdentifier::name(id), memberObject);
		});
	}

	if (object.isKindOf<OtArrayClass>()) {
		auto& items = variable.members.emplace_back("_items");
		auto array = OtArray(object)->raw();
		size_t index = 0;

		for (auto& arrayMember : array) {
			addObject(items.members, std::to_string(index++), arrayMember);
		}

	} else if (object.isKindOf<OtDictClass>()) {
		auto& items = variable.members.emplace_back("_items");
		auto dict = OtDict(object)->raw();

		for (auto& [memberName, memberObject] : dict) {
			addObject(items.members, memberName, memberObject);
		}

	} else if (object.isKindOf<OtSetClass>()) {
		auto& items = variable.members.emplace_back("_items");
		auto set = OtSet(object)->raw();
		size_t index = 0;

		for (auto& memberObject : set) {
			addObject(items.members, std::to_string(index++), memberObject);
		}
	}
}


//
//	OtDebugState::serializeVariable
//

nlohmann::json OtDebugState::serializeVariable(Variable& variable) {
	auto data = nlohmann::json::object();
	data["name"] = variable.name;
	data["type"] = variable.type;
	data["value"] = variable.value;

	if (variable.members.size()) {
		auto membersData = nlohmann::json::array();

		for (auto& memberVariable : variable.members) {
			membersData.push_back(serializeVariable(memberVariable));
		}

		data["members"] = membersData;
	}

	return data;
}


//
//	OtDebugState::deserializeVariable
//

void OtDebugState::deserializeVariable(std::vector<Variable>& list, nlohmann::json& json) {
	std::string name = json["name"];
	auto& variable = list.emplace_back(name, json["type"], json["value"]);

	if (json.contains("members")) {
		for (auto& variableData : json["members"]) {
			deserializeVariable(variable.members, variableData);
		}
	}
}
