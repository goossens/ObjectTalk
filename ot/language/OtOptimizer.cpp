//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <unordered_map>

#include "OtMemberReference.h"
#include "OtOptimizer.h"
#include "OtIdentifier.h"
#include "OtStackReference.h"


//
//	OtOptimizer::optimize
//

OtByteCode OtOptimizer::optimize(OtByteCode bytecode) {
	// remember the old bytecode and create a new one
	oldByteCode = bytecode;
	newByteCode = OtByteCode::create(bytecode->getSource(), bytecode->getName());

	// calculate the start position of each opcode in the old bytecode
	size_t pc = 0;
	size_t end = bytecode->size();

	while (pc < end) {
		// save all opcode starts
		opcodes.push_back(pc);
		pc += bytecode->getOpcodeSize(pc);
	}

	// process all opcodes and see if we have a sequence that can be optimized
	size_t opcode = 0;
	end = opcodes.size();

	std::unordered_map<size_t, size_t> opcodeMapping;

	while (opcode < end) {
		// update jump mapping
		opcodeMapping[opcodes[opcode]] = newByteCode->size();

		// try all optimization sequences (in the right order)
		bool optimized = false;
		if (!optimized) { optimized = optimizePushStackReferenceSequence(opcode, end - opcode); }
		if (!optimized) { optimized = optimizePushMemberReferenceSequence(opcode, end - opcode); }
		if (!optimized) { optimized = optimizePushMemberSequence(opcode, end - opcode); }
		if (!optimized) { optimized = optimizePushStackSwapAssignSequence(opcode, end - opcode); }
		if (!optimized) { optimized = optimizePushMemberSwapAssignSequence(opcode, end - opcode); }
		if (!optimized) { newByteCode->copyOpcode(oldByteCode, opcodes[opcode++]); }
	}

	// fix all the jumps
	for (auto& offset : newByteCode->getJumps()) {
		offset = opcodeMapping[offset];
	}

	// clone and fix statement index
	for (auto& statement : oldByteCode->getStatements()) {
		newByteCode->addStatement(
			statement.sourceStart, statement.sourceEnd,
			opcodeMapping[statement.opcodeStart], opcodeMapping[statement.opcodeEnd]);
	}

	// clone and fix symbol table
	for (auto& symbol : oldByteCode->getSymbols()) {
		symbol.opcodeStart = opcodeMapping[symbol.opcodeStart];
		symbol.opcodeEnd = opcodeMapping[symbol.opcodeEnd];
		newByteCode->addSymbol(symbol);
	}

	// capture optimized bytecode
	auto result = newByteCode;

	// clear our state
	oldByteCode = nullptr;
	newByteCode = nullptr;
	opcodes.clear();

	// return optimized code
	return result;
}


//
//	OtOptimizer::optimizePushStackReferenceSequence
//

bool OtOptimizer::optimizePushStackReferenceSequence(size_t& opcode, size_t available) {
	OtStackReference reference;

	if (available >= 2 &&
		oldByteCode->isPushStackReference(opcodes[opcode], reference) &&
		oldByteCode->isMethodDeref(opcodes[opcode + 1])) {

		newByteCode->pushStack(reference->getSlot());
		opcode += 2;
		return true;

	} else {
		return false;
	}
}


//
//	OtOptimizer::optimizePushMemberReferenceSequence
//

bool OtOptimizer::optimizePushMemberReferenceSequence(size_t& opcode, size_t available) {
	OtMemberReference reference;

	if (available >= 2 &&
		oldByteCode->isPushMemberReference(opcodes[opcode], reference) &&
		oldByteCode->isMethodDeref(opcodes[opcode + 1])) {

		newByteCode->pushObjectMember(reference->getObject(), reference->getMember());
		opcode += 2;
		return true;

	} else {
		return false;
	}
}


//
//	OtOptimizer::optimizePushMemberSequence
//

bool OtOptimizer::optimizePushMemberSequence(size_t& opcode, size_t available) {
	size_t member;

	if (available >= 2 &&
		oldByteCode->isMember(opcodes[opcode], member) &&
		oldByteCode->isMethodDeref(opcodes[opcode + 1])) {

		newByteCode->pushMember(member);
		opcode += 2;
		return true;

	} else {
		return false;
	}
}


//
//	OtOptimizer::optimizePushStackSwapAssignSequence
//

bool OtOptimizer::optimizePushStackSwapAssignSequence(size_t& opcode, size_t available) {
	OtObject object;
	OtStackReference reference;

	if (available >= 3 &&
		oldByteCode->isPushStackReference(opcodes[opcode], reference) &&
		oldByteCode->isSwap(opcodes[opcode + 1]) &&
		oldByteCode->isMethodAssign(opcodes[opcode + 2])) {

		newByteCode->assignStack(reference->getSlot());
		opcode += 3;
		return true;

	} else {
		return false;
	}
}


//
//	OtOptimizer::optimizePushMemberSwapAssignSequence
//

bool OtOptimizer::optimizePushMemberSwapAssignSequence(size_t& opcode, size_t available) {
	OtObject object;
	OtMemberReference reference;

	if (available >= 3 &&
		oldByteCode->isPushMemberReference(opcodes[opcode], reference) &&
		oldByteCode->isSwap(opcodes[opcode + 1]) &&
		oldByteCode->isMethodAssign(opcodes[opcode + 2])) {

		newByteCode->assignMember(reference->getObject(), reference->getMember());
		opcode += 3;
		return true;

	} else {
		return false;
	}
}
