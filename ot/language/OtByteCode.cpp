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

#include "fmt/format.h"

#include "OtByteCode.h"
#include "OtLog.h"
#include "OtMemberReference.h"
#include "OtSymbol.h"
#include "OtStackReference.h"
#include "OtText.h"


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

		switch (getOpcode(pc)) {
			case statementOpcode:
				buffer << "statementOpcode";
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

			case reserveOpcode:
				buffer << "reserve";
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

	return buffer.str();
}


//
//	OtByteCodeClass::copyOpcode
//

void OtByteCodeClass::copyOpcode(OtByteCode other, size_t pc) {
	switch (other->getOpcode(pc)) {
		case statementOpcode:
			statement();
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

		case reserveOpcode:
			reserve();
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
//	OtByteCodeClass::getOpcodeSize
//

size_t OtByteCodeClass::getOpcodeSize(size_t offset) {
	size_t pc = offset;

	// increment "program counter"
	switch (getOpcode(pc)) {
		case statementOpcode:
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

		case reserveOpcode:
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
//	OtByteCodeClass::getStatementStart
//

size_t OtByteCodeClass::getStatementStart(size_t pc) {
	// find statement
	for (auto& statement : statements) {
		if (pc >= statement.opcodeStart && pc < statement.opcodeEnd) {
			return statement.sourceStart;
		}
	}

	OtLogFatal("Internal error: can't find statement");

	// we'll never get here but it keeps the compiler happy
	return 0;
}

//
//	OtByteCodeClass::getStatementEnd
//

size_t OtByteCodeClass::getStatementEnd(size_t pc) {
	// find statement
	for (auto& statement : statements) {
		if (pc >= statement.opcodeStart && pc < statement.opcodeEnd) {
			return statement.sourceEnd;
		}
	}

	OtLogFatal("Internal error: can't find statement");

	// we'll never get here but it keeps the compiler happy
	return 0;
}


//
//	OtByteCodeClass::getLineNumber
//

size_t OtByteCodeClass::getLineNumber(size_t pc) {
	return source->getLineNumber(getStatementStart(pc));
}


//
//	OtByteCodeClass::getStatementSourceCode
//

std::string OtByteCodeClass::getStatementSourceCode(size_t pc) {
	// for start and end of statement
	auto start = getStatementStart(pc);
	auto end = getStatementEnd(pc);

	// get the line number of the start and all source code lines
	auto lineNo = source->getLineNumber(start);
	auto lines = source->getLines(start, end);

	// format nicely
	std::string statement;

	OtText::splitIterator(lines.data(), lines.data() + lines.size(), '\n', [&](const char* b, const char* e) {
		std::string text(b, e - b);

		if (statement.size()) {
			statement += '\n';
		}

		statement += fmt::format("Line {}: {}", lineNo++, text);
	});

	return statement;
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
