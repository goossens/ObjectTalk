//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "OtByteCode.h"


//
//	OtOptimizer
//

class OtOptimizer {
public:
	// constructor
	OtOptimizer();

	// optimize a bytecode sequenze and return a new version
	OtByteCode optimize(OtByteCode bytecode);

private:
	// optimize sequences of instructions
	bool optimizePushPushSwapAssignSequence(size_t& instruction);
	bool optimizePushMemberReferenceSequence(size_t& instruction);
	bool optimizeCallMemberSequence(size_t& instruction);
	bool optimizePushMemberSequence(size_t& instruction);

	// old and new bytecodes
	OtByteCode oldByteCode;
	OtByteCode newByteCode;

	// start of each instruction
	std::vector<size_t> instructions;
	void getInstructionOffsets(OtByteCode bytecode);

	// translation table to fix jumps
	std::unordered_map<size_t, size_t> jumpMapping;

	// selectors for frequently used items
	size_t derefSelector;
	size_t callSelector;
};
