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
#include <unordered_map>

#include "fmt/format.h"

#include "OtByteCode.h"
#include "OtLog.h"
#include "OtMemberReference.h"
#include "OtIdentifier.h"
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
				buffer << "statement";
				break;

			case pushOpcode: {
				buffer << "push" << OtObjectDescribe(constants[getNumber(pc)]);
				break;
			}

			case pushNullOpcode: {
				buffer << "pushNull";
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
				buffer << "jump" << jumps[getNumber(pc)];
				break;

			case jumpTrueOpcode:
				buffer << "jumpTrue" << jumps[getNumber(pc)];
				break;

			case jumpFalseOpcode:
				buffer << "jumpTrue" << jumps[getNumber(pc)];
				break;

			case memberOpcode:
				buffer << "member" << OtIdentifier::name(getID(pc));
				break;

			case methodOpcode: {
				auto method = getID(pc);
				auto count =getNumber(pc);
				buffer << "method" << OtIdentifier::name(method) << "(" << count << ")";
				break;
			}

			case superOpcode:
				buffer << "super" << OtIdentifier::name(getID(pc));
				break;

			case exitOpcode:
				buffer << "exit";
				break;

			case pushTryOpcode:
				buffer << "pushTry" << jumps[getNumber(pc)];
				break;

			case popTryOpcode:
				buffer << "popTry";
				break;

			case pushStackObjectOpcode:
				buffer << "pushStackObject" << getNumber(pc);
				break;

			case pushStackMemberOpcode: {
				auto offset = getNumber(pc);
				auto memberName = OtIdentifier::name(getID(pc));
				buffer << "pushStackMember" << offset << " " << memberName;
				break;
			}

			case pushObjectMemberOpcode: {
				auto objectName = OtObjectDescribe(constants[getNumber(pc)]);
				auto memberName = OtIdentifier::name(getID(pc));
				buffer << "pushObjectMember" << objectName << " " << memberName;
				break;
			}

			case pushMemberOpcode:
				buffer << "pushMember" << OtIdentifier::name(getID(pc));
				break;

			case assignStackOpcode:
				buffer << "assignStack" << getNumber(pc);
				break;

			case assignMemberOpcode: {
				auto objectName = OtObjectDescribe(constants[getNumber(pc)]);
				auto memberName = OtIdentifier::name(getID(pc));
				buffer << "assignMember" << objectName << " " << memberName;
				break;
			}
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

		case pushNullOpcode:
			pushNull();
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
			jump(other->jumps[other->getNumber(pc)]);
			break;

		case jumpTrueOpcode:
			jumpTrue(other->jumps[other->getNumber(pc)]);
			break;

		case jumpFalseOpcode:
			jumpFalse(other->jumps[other->getNumber(pc)]);
			break;

		case memberOpcode:
			member(other->getID(pc));
			break;

		case superOpcode:
			super(other->getID(pc));
			break;

		case methodOpcode: {
			auto id = other->getID(pc);
			auto count = other->getNumber(pc);
			method(id, count);
			break;
		}

		case exitOpcode:
			exit();
			break;

		case pushTryOpcode:
			pushTry(other->jumps[other->getNumber(pc)]);
			break;

		case popTryOpcode:
			popTry();
			break;

		case pushStackObjectOpcode:
			pushStackObject(other->getNumber(pc));
			break;

		case pushStackMemberOpcode: {
			auto slot = other->getNumber(pc);
			auto member = other->getID(pc);
			pushStackMember(slot, member);
			break;
		}

		case pushObjectMemberOpcode: {
			auto object = other->getNumber(pc);
			auto member = other->getID(pc);
			pushObjectMember(other->constants[object], member);
			break;
		}

		case pushMemberOpcode: {
			auto member = other->getID(pc);
			pushMember(member);
			break;
		}

		case assignStackOpcode:
			assignStack(other->getNumber(pc));
			break;

		case assignMemberOpcode: {
			auto object = other->getNumber(pc);
			auto member = other->getID(pc);
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

		case pushNullOpcode:
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

		case pushTryOpcode:
			getNumber(pc);
			break;

		case popTryOpcode:
			break;

		case pushStackObjectOpcode:
			getNumber(pc);
			break;

		case pushStackMemberOpcode:
			getNumber(pc);
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

bool OtByteCodeClass::isMember(size_t pc, OtID& member) {
	if (getOpcode(pc) == memberOpcode) {
		member = getID(pc);
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
		return  getNumber(pc) == dereferenceID;

	} else {
		return false;
	}
}


//
//	OtByteCodeClass::isMethodAssign
//

bool OtByteCodeClass::isMethodAssign(size_t pc) {
	if (getOpcode(pc) == methodOpcode) {
		return  getNumber(pc) == assignID;

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
//	OtByteCodeClass::getUsedSymbols
//

std::vector<OtSymbol> OtByteCodeClass::getUsedSymbols(size_t pc) {
	std::vector<OtSymbol> syms;
	std::unordered_map<size_t, bool> index;

	for (auto& symbol : symbols) {
		if (pc >= symbol.opcodeStart && pc < symbol.opcodeEnd && index.count(symbol.id) == 0) {
			syms.emplace_back(symbol);
			index[symbol.id] = true;
		}
	}

	return syms;
}


//
//	OtByteCodeClass::getUsedSymbolNames
//

std::vector<std::string> OtByteCodeClass::getUsedSymbolNames(size_t pc) {
	std::vector<std::string> names;
	std::unordered_map<size_t, bool> index;

	for (auto& symbol : symbols) {
		if (pc >= symbol.opcodeStart && pc < symbol.opcodeEnd && index.count(symbol.id) == 0) {
			names.emplace_back(OtIdentifier::name(symbol.id));
			index[symbol.id] = true;
		}
	}

	return names;
}


//
//	OtByteCodeClass::getMeta
//

OtType OtByteCodeClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtByteCodeClass>("ByteCode", OtInternalClass::getMeta());
	}

	return type;
}
