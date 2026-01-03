//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <string_view>

#include "nlohmann/json_fwd.hpp"

#include "OtObject.h"


//
//	OtDebugState
//

class OtDebugState {
public:
	// update the state
	void update();

	// (de)serialize content
	std::string serialize();
	void deserialize(const std::string& string);

	// properties
	class Variable {
	public:
		// constructor
		Variable(const std::string& n) : name(n) {}
		Variable(const std::string_view& n, const std::string& t, const std::string& v) : name(std::string(n)), type(t), value(v) {}

		// properties
		std::string name;
		std::string type;
		std::string value;
		std::vector<Variable> members;
	};

	class Frame {
	public:
		// constructor
		Frame(const std::string& m, size_t l) : module(m), line(l) {}

		// properties
		std::string module;
		size_t line;
		std::vector<Variable> variables;
	};

	std::vector<Frame> frames;

private:
	// add objects as variable
	void addObject(std::vector<Variable>& list, const std::string_view& name, OtObject object);

	// (de)serialize variables
	nlohmann::json serializeVariable(Variable& variable);
	void deserializeVariable(std::vector<Variable>& list, nlohmann::json& json);
};
