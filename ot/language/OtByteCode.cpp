//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "OtByteCode.h"
#include "OtMemberReference.h"
#include "OtSymbol.h"
#include "OtStackReference.h"


//
//	OtByteCodeClass::disassemble
//

std::string OtByteCodeClass::disassemble() {
	std::stringstream buffer;
	size_t pc = 0;
	size_t end = bytecode.size();

	while (pc < end) {
		buffer << std::right;
		buffer << std::setw(4) << std::setfill('0') << pc << std::setfill(' ') << " ";
		buffer << std::left << std::setw(20);

		switch (bytecode[pc++]) {
			case debugOpcode:
				buffer << "debug";
				break;

			case markOpcode:
				buffer << "mark" << marks[getNumber(pc)];
				break;

			case pushOpcode: {
				buffer << "push" << OtObjectDescribe(constants[getNumber(pc)]);
				break;
			}

			case popOpcode:
				buffer << "pop";
				break;

			case popCountOpcode:
				buffer << "popCount" << getNumber(pc);
				break;

			case dupOpcode:
				buffer << "dup";
				break;

			case swapOpcode:
				buffer << "swap";
				break;

			case moveOpcode:
				buffer << "move" << getNumber(pc);
				break;

			case reserveOpcode:
				buffer << "reserve";
				break;

			case jumpOpcode:
				buffer << "jump" << offsets[getNumber(pc)];
				break;

			case jumpTrueOpcode:
				buffer << "jumpTrue" << offsets[getNumber(pc)];
				break;

			case jumpFalseOpcode:
				buffer << "jumpTrue" << offsets[getNumber(pc)];
				break;

			case memberOpcode:
				buffer << "member" << OtSymbol::name(getNumber(pc));
				break;

			case methodOpcode: {
				auto method = getNumber(pc);
				auto count =getNumber(pc);
				buffer << "method" << OtSymbol::name(method) << "(" << count << ")";
				break;
			}

			case superOpcode:
				buffer << "super" << OtSymbol::name(getNumber(pc));
				break;

			case exitOpcode:
				buffer << "exit";
				break;

			case pushTryOpcode:
				buffer << "pushTry" << offsets[getNumber(pc)];
				break;

			case popTryOpcode:
				buffer << "popTry";
				break;

			case pushStackOpcode:
				buffer << "pushStack" << getNumber(pc);
				break;

			case pushObjectMemberOpcode:
				buffer << "pushObjectMember" << OtObjectDescribe(constants[getNumber(pc)]) << " " << OtSymbol::name(getNumber(pc));
				break;

			case pushMemberOpcode:
				buffer << "pushMember" << OtSymbol::name(getNumber(pc));
				break;

			case assignStackOpcode:
				buffer << "assignStack" << getNumber(pc);
				break;

			case assignMemberOpcode:
				buffer << "assignMember" << OtObjectDescribe(constants[getNumber(pc)]) << " " << OtSymbol::name(getNumber(pc));
				break;
		}

		buffer << std::endl;
	}

	buffer << std::endl;

	buffer << "bytecode: " << bytecode.size() << std::endl;
	buffer << "constants: " << constants.size() << std::endl;
	buffer << "marks: " << marks.size() << std::endl;
	buffer << "offsets: " << offsets.size() << std::endl;

	return buffer.str();
}


//
//	OtByteCodeClass::copyInstruction
//

void OtByteCodeClass::copyInstruction(OtByteCode other, size_t pc) {
	switch (other->bytecode[pc++]) {
		case debugOpcode:
			debug();
			break;

		case markOpcode:
			mark(other->marks[other->getNumber(pc)]);
			break;

		case pushOpcode:
			push(other->constants[other->getNumber(pc)]);
			break;

		case popOpcode:
			pop();
			break;

		case popCountOpcode:
			pop(other->getNumber(pc));
			break;

		case dupOpcode:
			dup();
			break;

		case swapOpcode:
			swap();
			break;

		case moveOpcode:
			move(other->getNumber(pc));
			break;

		case reserveOpcode:
			reserve();
			break;

		case jumpOpcode:
			jump(other->offsets[other->getNumber(pc)]);
			break;

		case jumpTrueOpcode:
			jumpTrue(other->offsets[other->getNumber(pc)]);
			break;

		case jumpFalseOpcode:
			jumpFalse(other->offsets[other->getNumber(pc)]);
			break;

		case memberOpcode:
			member(other->getNumber(pc));
			break;

		case superOpcode:
			super(other->getNumber(pc));
			break;

		case methodOpcode: {
			auto symbol = other->getNumber(pc);
			auto count = other->getNumber(pc);
			method(symbol, count);
			break;
		}

		case exitOpcode:
			exit();
			break;

		case pushTryOpcode:
			pushTry(other->offsets[other->getNumber(pc)]);
			break;

		case popTryOpcode:
			popTry();
			break;

		case pushStackOpcode:
			pushStack(other->getNumber(pc));
			break;

		case pushObjectMemberOpcode: {
			auto object = other->getNumber(pc);
			auto member = other->getNumber(pc);
			pushObjectMember(other->constants[object], member);
			break;
		}

		case pushMemberOpcode: {
			auto member = other->getNumber(pc);
			pushMember(member);
			break;
		}

		case assignStackOpcode:
			assignStack(other->getNumber(pc));
			break;

		case assignMemberOpcode: {
			auto object = other->getNumber(pc);
			auto member = other->getNumber(pc);
			assignMember(other->constants[object], member);
			break;
		}
	}
}


//
//	OtByteCodeClass::getInstructionSize
//

size_t OtByteCodeClass::getInstructionSize(size_t offset) {
	size_t pc = offset;

	// increment "program counter"
	switch (bytecode[pc++]) {
		case debugOpcode:
			break;

		case markOpcode:
			getNumber(pc);
			break;

		case pushOpcode:
			getNumber(pc);
			break;

		case popOpcode:
			break;

		case popCountOpcode:
			getNumber(pc);
			break;

		case dupOpcode:
			break;

		case swapOpcode:
			break;

		case moveOpcode:
			getNumber(pc);
			break;

		case reserveOpcode:
			break;

		case jumpOpcode:
			getNumber(pc);
			break;

		case jumpTrueOpcode:
			getNumber(pc);
			break;

		case jumpFalseOpcode:
			getNumber(pc);
			break;

		case memberOpcode:
			getNumber(pc);
			break;

		case methodOpcode:
			getNumber(pc);
			getNumber(pc);
			break;

		case superOpcode:
			getNumber(pc);
			break;

		case exitOpcode:
			break;

		case pushTryOpcode:
			getNumber(pc);
			break;

		case popTryOpcode:
			break;

		case pushStackOpcode:
			getNumber(pc);
			break;

		case pushObjectMemberOpcode:
			getNumber(pc);
			getNumber(pc);
			break;

		case pushMemberOpcode:
			getNumber(pc);
			break;

		case assignStackOpcode:
			getNumber(pc);
			break;

		case assignMemberOpcode:
			getNumber(pc);
			getNumber(pc);
			break;
	}

	return pc - offset;
}


//
//	OtByteCodeClass::isPush
//

bool OtByteCodeClass::isPush(size_t pc, OtObject& object) {
	if (getOpcode(pc) == pushOpcode) {
		object = constants[getNumber(pc)];
		return true;

	} else {
		return false;
	}
}


//
//	OtByteCodeClass::isPushStackReference
//

bool OtByteCodeClass::isPushStackReference(size_t pc, OtStackReference& reference) {
	if (getOpcode(pc) == pushOpcode) {
		OtObject object = constants[getNumber(pc)];

		if (object.isKindOf<OtStackReferenceClass>()) {
			reference = OtStackReference(object);
			return true;

		} else {
			return false;
		}

	} else {
		return false;
	}
}


//
//
//	OtByteCodeClass::isPushMemberReference
//

bool OtByteCodeClass::isPushMemberReference(size_t pc, OtMemberReference& reference) {
	if (getOpcode(pc) == pushOpcode) {
		OtObject object = constants[getNumber(pc)];

		if (object.isKindOf<OtMemberReferenceClass>()) {
			reference = OtMemberReference(object);
			return true;

		} else {
			return false;
		}

	} else {
		return false;
	}
}


//
//	OtByteCodeClass::isSwap
//

bool OtByteCodeClass::isSwap(size_t pc) {
	return getOpcode(pc) == swapOpcode;
}


//
//	OtByteCodeClass::isMember
//

bool OtByteCodeClass::isMember(size_t pc, size_t& member) {
	if (getOpcode(pc) == memberOpcode) {
		member = getNumber(pc);
		return true;

	} else {
		return false;
	}
}


//
//	OtByteCodeClass::isMethodDeref
//

bool OtByteCodeClass::isMethodDeref(size_t pc) {
	if (getOpcode(pc) == methodOpcode) {
		return  getNumber(pc) == OtSymbol::create("__deref__");

	} else {
		return false;
	}
}


//
//	OtByteCodeClass::isMethodAssign
//

bool OtByteCodeClass::isMethodAssign(size_t pc) {
	if (getOpcode(pc) == methodOpcode) {
		return  getNumber(pc) == OtSymbol::create("__assign__");

	} else {
		return false;
	}
}


//
//	OtByteCodeClass::isAnyJump
//

bool OtByteCodeClass::isAnyJump(size_t pc, size_t& offset) {
	auto opcode = getOpcode(pc);

	if (opcode == jumpOpcode || opcode == jumpTrueOpcode || opcode == jumpFalseOpcode) {
		offset = getNumber(pc);
		return true;

	} else {
		return false;
	}
}


//
//	OtByteCodeClass::getMeta
//

OtType OtByteCodeClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtByteCodeClass>("Code", OtInternalClass::getMeta());
	}

	return type;
}
