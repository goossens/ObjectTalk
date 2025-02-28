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
#include "OtVM.h"


//
//	OtDebugState::update
//

void OtDebugState::update() {
	auto bytecode = OtVM::getByteCode();
	auto pc = OtVM::getPC();
	auto stack = OtVM::getStack();

	module = bytecode->getModule();
	line = bytecode->getLineNumber(pc);

	variables.clear();
	auto symbols = bytecode->getUsedSymbols(pc);

	for (auto& symbol : symbols) {
		auto name = OtIdentifier::name(symbol.id);

		switch (symbol.type) {
			case OtSymbol::Type::heap:
			addObject(variables, name, symbol.object->get(symbol.id));
				break;

			case OtSymbol::Type::stack:
			addObject(variables, name, OtVM::getStack()->getFrameItem(symbol.slot));
				break;

			case OtSymbol::Type::capture:
				break;
		}
	}

	for (size_t i = 0; i < stack->getFrameCount(); i++) {
		auto& stackframe = stack->getFrame(i);
		stackframes.emplace_back(stackframe.bytecode->getModule(), stackframe.bytecode->getLineNumber(pc));
	}
}


//
//	OtDebugState::serialize
//

std::string OtDebugState::serialize() {
	auto variableList = nlohmann::json::array();
	auto stackFrameList = nlohmann::json::array();

	for (auto& variable : variables) {
		variableList.push_back(serializeVariable(variable));
	}

	for (auto& stackframe : stackframes) {
		stackFrameList.push_back(serializeStackFrame(stackframe));
	}

	auto data = nlohmann::json::object();
	data["module"] = module;
	data["line"] = line;
	data["variables"] = variableList;
	data["stackframes"] = stackFrameList;
	return data.dump();
}


//
//	OtDebugState::deserialize
//

void OtDebugState::deserialize(const std::string& string) {
	auto data = nlohmann::json::parse(string);
	module = data.value("module", "unknown");
	line = data.value("line", 0);
	variables.clear();

	for (auto& variableData : data["variables"]) {
		deserializeVariable(variables, variableData);
	}

	for (auto& stackFrameData : data["stackframes"]) {
		deserializeStackFrame(stackframes, stackFrameData);
	}
}


//
//	OtDebugState::addObject
//

void OtDebugState::addObject(std::vector<Variable>& list, const std::string_view& name, OtObject object) {
	auto& variable = list.emplace_back(name, object->getType()->getName(), object->describe());

	if (object->hasMembers()) {
		object->eachMember([this, &variable](OtID id, OtObject memberObject) {
			addObject(variable.members, OtIdentifier::name(id), memberObject);
		});
	}

	if (object->isKindOf("Array")) {
		auto& items = variable.members.emplace_back("_items");
		auto array = OtArray(object)->raw();
		size_t index = 0;

		for (auto& arrayMember : array) {
			addObject(items.members, std::to_string(index++), arrayMember);
		}

	} else if (object->isKindOf("Dict")) {
		auto& items = variable.members.emplace_back("_items");
		auto dict = OtDict(object)->raw();

		for (auto& [memberName, memberObject] : dict) {
			addObject(items.members, memberName, memberObject);
		}

	} else if (object->isKindOf("Set")) {
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


//
//	OtDebugState::serializeStackFrame
//

nlohmann::json OtDebugState::serializeStackFrame(StackFrame& stackframe) {
	auto data = nlohmann::json::object();
	data["module"] = stackframe.module;
	data["line"] = stackframe.line;
	return data;
}


//
//	OtDebugState::deserializeStackFrame
//

void OtDebugState::deserializeStackFrame(std::vector<StackFrame>& list, nlohmann::json& json) {
	list.emplace_back(json["module"], json["line"]);
}
