//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
	bool optimizePushStackReferenceSequence(size_t& instruction, size_t available);
	bool optimizePushMemberReferenceSequence(size_t& instruction, size_t available);
	bool optimizePushMemberSequence(size_t& instruction, size_t available);
	bool optimizePushStackSwapAssignSequence(size_t& instruction, size_t available);
	bool optimizePushMemberSwapAssignSequence(size_t& instruction, size_t available);

	// old and new bytecodes
	OtByteCode oldByteCode;
	OtByteCode newByteCode;

	// start of each instruction
	std::vector<size_t> instructions;

	// translation table to fix jumps
	std::unordered_map<size_t, size_t> jumpMapping;

	// symbols for frequently used items
	size_t derefSymbol;
	size_t callSymbol;
};
