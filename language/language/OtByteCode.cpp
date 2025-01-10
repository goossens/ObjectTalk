//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
			case Opcode::statement:
				buffer << "statement";
				break;

			case Opcode::push: {
				buffer << "push" << OtObjectDescribe(constants[getNumber(pc)]);
				break;
			}

			case Opcode::pushNull: {
				buffer << "pushNull";
				break;
			}

			case Opcode::pop:
				buffer << "pop";
				break;

			case Opcode::popCount:
				buffer << "popCount" << getNumber(pc);
				break;

			case Opcode::dup:
				buffer << "dup";
				break;

			case Opcode::swap:
				buffer << "swap";
				break;

			case Opcode::move:
				buffer << "move" << getNumber(pc);
				break;

			case Opcode::jump:
				buffer << "jump" << jumps[getNumber(pc)];
				break;

			case Opcode::jumpTrue:
				buffer << "jumpTrue" << jumps[getNumber(pc)];
				break;

			case Opcode::jumpFalse:
				buffer << "jumpTrue" << jumps[getNumber(pc)];
				break;

			case Opcode::member:
				buffer << "member" << OtIdentifier::name(getID(pc));
				break;

			case Opcode::method: {
				auto method = getID(pc);
				auto count =getNumber(pc);
				buffer << "method" << OtIdentifier::name(method) << "(" << count << ")";
				break;
			}

			case Opcode::super:
				buffer << "super" << OtIdentifier::name(getID(pc));
				break;

			case Opcode::exit:
				buffer << "exit";
				break;

			case Opcode::pushTry:
				buffer << "pushTry" << jumps[getNumber(pc)];
				break;

			case Opcode::popTry:
				buffer << "popTry";
				break;

			case Opcode::pushStackObject:
				buffer << "pushStackObject" << getNumber(pc);
				break;

			case Opcode::pushStackMember: {
				auto offset = getNumber(pc);
				auto memberName = OtIdentifier::name(getID(pc));
				buffer << "pushStackMember" << offset << " " << memberName;
				break;
			}

			case Opcode::pushObjectMember: {
				auto objectName = OtObjectDescribe(constants[getNumber(pc)]);
				auto memberName = OtIdentifier::name(getID(pc));
				buffer << "pushObjectMember" << objectName << " " << memberName;
				break;
			}

			case Opcode::pushMember:
				buffer << "pushMember" << OtIdentifier::name(getID(pc));
				break;

			case Opcode::assignStack:
				buffer << "assignStack" << getNumber(pc);
				break;

			case Opcode::assignMember: {
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
		case Opcode::statement:
			statement();
			break;

		case Opcode::push:
			push(other->constants[other->getNumber(pc)]);
			break;

		case Opcode::pushNull:
			pushNull();
			break;

		case Opcode::pop:
			pop();
			break;

		case Opcode::popCount:
			pop(other->getNumber(pc));
			break;

		case Opcode::dup:
			dup();
			break;

		case Opcode::swap:
			swap();
			break;

		case Opcode::move:
			move(other->getNumber(pc));
			break;

		case Opcode::jump:
			jump(other->jumps[other->getNumber(pc)]);
			break;

		case Opcode::jumpTrue:
			jumpTrue(other->jumps[other->getNumber(pc)]);
			break;

		case Opcode::jumpFalse:
			jumpFalse(other->jumps[other->getNumber(pc)]);
			break;

		case Opcode::member:
			member(other->getID(pc));
			break;

		case Opcode::super:
			super(other->getID(pc));
			break;

		case Opcode::method: {
			auto id = other->getID(pc);
			auto count = other->getNumber(pc);
			method(id, count);
			break;
		}

		case Opcode::exit:
			exit();
			break;

		case Opcode::pushTry:
			pushTry(other->jumps[other->getNumber(pc)]);
			break;

		case Opcode::popTry:
			popTry();
			break;

		case Opcode::pushStackObject:
			pushStackObject(other->getNumber(pc));
			break;

		case Opcode::pushStackMember: {
			auto slot = other->getNumber(pc);
			auto member = other->getID(pc);
			pushStackMember(slot, member);
			break;
		}

		case Opcode::pushObjectMember: {
			auto object = other->getNumber(pc);
			auto member = other->getID(pc);
			pushObjectMember(other->constants[object], member);
			break;
		}

		case Opcode::pushMember: {
			auto member = other->getID(pc);
			pushMember(member);
			break;
		}

		case Opcode::assignStack:
			assignStack(other->getNumber(pc));
			break;

		case Opcode::assignMember: {
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
		case Opcode::statement:
			break;

		case Opcode::push:
			getNumber(pc);
			break;

		case Opcode::pushNull:
			break;

		case Opcode::pop:
			break;

		case Opcode::popCount:
			getNumber(pc);
			break;

		case Opcode::dup:
			break;

		case Opcode::swap:
			break;

		case Opcode::move:
			getNumber(pc);
			break;

		case Opcode::jump:
			getNumber(pc);
			break;

		case Opcode::jumpTrue:
			getNumber(pc);
			break;

		case Opcode::jumpFalse:
			getNumber(pc);
			break;

		case Opcode::member:
			getNumber(pc);
			break;

		case Opcode::method:
			getNumber(pc);
			getNumber(pc);
			break;

		case Opcode::super:
			getNumber(pc);
			break;

		case Opcode::exit:
			break;

		case Opcode::pushTry:
			getNumber(pc);
			break;

		case Opcode::popTry:
			break;

		case Opcode::pushStackObject:
			getNumber(pc);
			break;

		case Opcode::pushStackMember:
			getNumber(pc);
			getNumber(pc);
			break;

		case Opcode::pushObjectMember:
			getNumber(pc);
			getNumber(pc);
			break;

		case Opcode::pushMember:
			getNumber(pc);
			break;

		case Opcode::assignStack:
			getNumber(pc);
			break;

		case Opcode::assignMember:
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
	if (getOpcode(pc) == Opcode::push) {
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
	if (getOpcode(pc) == Opcode::push) {
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
	if (getOpcode(pc) == Opcode::push) {
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
	return getOpcode(pc) == Opcode::swap;
}


//
//	OtByteCodeClass::isMember
//

bool OtByteCodeClass::isMember(size_t pc, OtID& member) {
	if (getOpcode(pc) == Opcode::member) {
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
	if (getOpcode(pc) == Opcode::method) {
		return  getNumber(pc) == dereferenceID;

	} else {
		return false;
	}
}


//
//	OtByteCodeClass::isMethodAssign
//

bool OtByteCodeClass::isMethodAssign(size_t pc) {
	if (getOpcode(pc) == Opcode::method) {
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

	if (opcode == Opcode::jump || opcode == Opcode::jumpTrue || opcode == Opcode::jumpFalse) {
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
