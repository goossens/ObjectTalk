//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtMemberReference.h"
#include "OtOptimizer.h"
#include "OtSymbol.h"
#include "OtStackReference.h"


//
//	OtOptimizer::OtOptimizer
//

OtOptimizer::OtOptimizer() {
	derefSymbol = OtSymbol::create("__deref__");
	callSymbol = OtSymbol::create("__call__");
}


//
//	OtOptimizer::optimize
//

OtByteCode OtOptimizer::optimize(OtByteCode bytecode) {
	// remember the old bytecode and create a new one
	oldByteCode = bytecode;
	newByteCode = OtByteCode::create(bytecode->getSource(), bytecode->getName());

	// calculate the start position of each instruction in the old bytecode
	size_t pc = 0;
	size_t end = bytecode->size();

	while (pc < end) {
		// save all instruction starts
		instructions.push_back(pc);
		pc += bytecode->getInstructionSize(pc);
	}

	// process all instructions and see if we have a sequence that can be optimized
	size_t instruction = 0;
	end = instructions.size();

	while (instruction < end) {
		// update jump mapping
		jumpMapping[instructions[instruction]] = newByteCode->size();

		// try all optimization sequences (in the right order)
		bool optimized = false;
		if (!optimized) { optimized = optimizePushStackReferenceSequence(instruction, end - instruction); }
		if (!optimized) { optimized = optimizePushMemberReferenceSequence(instruction, end - instruction); }
		if (!optimized) { optimized = optimizePushMemberSequence(instruction, end - instruction); }
		if (!optimized) { optimized = optimizePushStackSwapAssignSequence(instruction, end - instruction); }
		if (!optimized) { optimized = optimizePushMemberSwapAssignSequence(instruction, end - instruction); }
		if (!optimized) { newByteCode->copyInstruction(oldByteCode, instructions[instruction++]); }
	}

	// fix all the jumps
	for (auto& offset : newByteCode->getOffsets()) {
		offset = jumpMapping[offset];
	}

	// capture optimized bytecode
	auto result = newByteCode;

	// clear the optimizer state
	OtByteCode oldByteCode = nullptr;
	OtByteCode newByteCode = nullptr;
	instructions.clear();
	jumpMapping.clear();

	// get start of each instruction
	return result;
}


//
//	OtOptimizer::optimizePushStackReferenceSequence
//

bool OtOptimizer::optimizePushStackReferenceSequence(size_t& instruction, size_t available) {
	OtStackReference reference;

	if (available >= 2 &&
		oldByteCode->isPushStackReference(instructions[instruction], reference) &&
		oldByteCode->isMethodDeref(instructions[instruction + 1])) {

		newByteCode->pushStack(reference->getSlot());
		instruction += 2;
		return true;

	} else {
		return false;
	}
}


//
//	OtOptimizer::optimizePushMemberReferenceSequence
//

bool OtOptimizer::optimizePushMemberReferenceSequence(size_t& instruction, size_t available) {
	OtMemberReference reference;

	if (available >= 2 &&
		oldByteCode->isPushMemberReference(instructions[instruction], reference) &&
		oldByteCode->isMethodDeref(instructions[instruction + 1])) {

		newByteCode->pushObjectMember(reference->getObject(), reference->getMember());
		instruction += 2;
		return true;

	} else {
		return false;
	}
}


//
//	OtOptimizer::optimizePushMemberSequence
//

bool OtOptimizer::optimizePushMemberSequence(size_t& instruction, size_t available) {
	size_t member;

	if (available >= 2 &&
		oldByteCode->isMember(instructions[instruction], member) &&
		oldByteCode->isMethodDeref(instructions[instruction + 1])) {

		newByteCode->pushMember(member);
		instruction += 2;
		return true;

	} else {
		return false;
	}
}


//
//	OtOptimizer::optimizePushStackSwapAssignSequence
//

bool OtOptimizer::optimizePushStackSwapAssignSequence(size_t& instruction, size_t available) {
	OtObject object;
	OtStackReference reference;

	if (available >= 3 &&
		oldByteCode->isPushStackReference(instructions[instruction], reference) &&
		oldByteCode->isSwap(instructions[instruction + 1]) &&
		oldByteCode->isMethodAssign(instructions[instruction + 2])) {

		newByteCode->assignStack(reference->getSlot());
		instruction += 3;
		return true;

	} else {
		return false;
	}
}


//
//	OtOptimizer::optimizePushMemberSwapAssignSequence
//

bool OtOptimizer::optimizePushMemberSwapAssignSequence(size_t& instruction, size_t available) {
	OtObject object;
	OtMemberReference reference;

	if (available >= 3 &&
		oldByteCode->isPushMemberReference(instructions[instruction], reference) &&
		oldByteCode->isSwap(instructions[instruction + 1]) &&
		oldByteCode->isMethodAssign(instructions[instruction + 2])) {

		newByteCode->assignMember(reference->getObject(), reference->getMember());
		instruction += 3;
		return true;

	} else {
		return false;
	}
}
