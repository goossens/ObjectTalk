//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtMemberReference.h"
#include "OtOptimizer.h"
#include "OtSelector.h"


//
//	OtOptimizer::OtOptimizer
//

OtOptimizer::OtOptimizer() {
	derefSelector = OtSelector::create("__deref__");
	callSelector = OtSelector::create("__call__");
}


//
//	OtOptimizer::optimize
//

OtByteCode OtOptimizer::optimize(OtByteCode bytecode) {
	// remember the old bytecode and create a new one
	oldByteCode = bytecode;
	newByteCode = OtByteCode::create(bytecode->getSource());

	// calculate the start position of each instruction in the old bytecode
	getInstructionOffsets(oldByteCode);

	// process all instructions and see if we have a sequence that can be optimized
	size_t instruction = 0;
	size_t end = instructions.size();

	while (instruction < end) {
		// update jump mapping
		jumpMapping[instructions[instruction]] = newByteCode->size();

		// try all optimization sequences (in the right order)
		bool optimized = false;
		if (!optimized) { optimized = optimizePushPushSwapAssignSequence(instruction, end - instruction); }
		if (!optimized) { optimized = optimizePushMemberReferenceSequence(instruction, end - instruction); }
		if (!optimized) { optimized = optimizeCallMemberSequence(instruction, end - instruction); }
		if (!optimized) { optimized = optimizePushMemberSequence(instruction, end - instruction); }
		if (!optimized) { newByteCode->copyInstruction(oldByteCode, instructions[instruction++]); }
	}

	// fix all the jumps
	for (auto& offset : newByteCode->getOffsets()) {
		offset = jumpMapping[offset];
	}

	// get start of each instruction
	return newByteCode;
}


//
//	OtOptimizer::optimizePushPushSwapAssignSequence
//

bool OtOptimizer::optimizePushPushSwapAssignSequence(size_t& instruction, size_t available) {
	OtObject object;
	OtMemberReference reference;

	if (available >= 4 &&
		oldByteCode->isPush(instructions[instruction], object) &&
		oldByteCode->isPushMemberReference(instructions[instruction + 1], reference) &&
		oldByteCode->isSwap(instructions[instruction + 2]) &&
		oldByteCode->isMethodAssign(instructions[instruction + 3])) {

		newByteCode->assignMember(reference->getObject(), reference->getMember(), object);
		instruction += 4;
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
//	OtOptimizer::optimizeCallMemberSequence
//

bool OtOptimizer::optimizeCallMemberSequence(size_t& instruction, size_t available) {
	size_t member;
	size_t count;

	if (available >= 3 &&
		oldByteCode->isMember(instructions[instruction], member) &&
		oldByteCode->isMethodDeref(instructions[instruction + 1]) &&
		oldByteCode->isMethodCall(instructions[instruction + 2], count)) {

		newByteCode->method(member, count);
		instruction += 3;
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
//	OtOptimizer::getInstructionOffsets
//

void OtOptimizer::getInstructionOffsets(OtByteCode bytecode) {
	// clear instruction offset list
	instructions.clear();

	// process all instructions
	size_t pc = 0;
	size_t end = bytecode->size();

	while (pc < end) {
		// save all instruction starts
		instructions.push_back(pc);
		pc += bytecode->getInstructionSize(pc);
	}
}
