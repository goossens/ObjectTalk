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
#include <vector>

#include "OtByteCode.h"


//
//	OtOptimizer
//

class OtOptimizer {
public:
	// optimize a bytecode sequenze and return a new version
	OtByteCode optimize(OtByteCode bytecode);

private:
	// optimize sequences of opcodes
	bool optimizePushStackMemberReferenceSequence(size_t& opcode, size_t available);
	bool optimizePushStackReferenceSequence(size_t& opcode, size_t available);
	bool optimizePushMemberReferenceSequence(size_t& opcode, size_t available);
	bool optimizePushMemberSequence(size_t& opcode, size_t available);
	bool optimizePushStackSwapAssignSequence(size_t& opcode, size_t available);
	bool optimizePushMemberSwapAssignSequence(size_t& opcode, size_t available);

	// old and new bytecodes
	OtByteCode oldByteCode;
	OtByteCode newByteCode;

	// start of each opcode
	std::vector<size_t> opcodes;
};
