//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "ot/bytecode.h"


//
//	OtByteCodeClass::disassemble
//

std::string OtByteCodeClass::disassemble() {
	std::stringstream buffer;
	size_t pc = 0;
	size_t end = bytecode.size();

	buffer << "Code:" << std::endl;
	buffer << "-----" << std::endl << std::endl;

	while (pc < end) {
		buffer << std::right;
		buffer << std::setw(4) << std::setfill('0') << pc << std::setfill(' ') << " ";
		buffer << std::left << std::setw(16);

		switch (bytecode[pc++]) {
			case DEBUG:
				buffer << "DEBUG";
				break;

			case MARK:
				buffer << "MARK" << marks[getNumber(pc)];
				break;

			case PUSH: {
				auto n = getNumber(pc);
				buffer << "PUSH" << OtObjectDescribe(constants[n]);
				break;
			}

			case POP:
				buffer << "POP";
				break;

			case POP_COUNT:
				buffer << "POP_COUNT" << getNumber(pc);
				break;

			case DUP:
				buffer << "DUP";
				break;

			case SWAP:
				buffer << "SWAP";
				break;

			case MOVE:
				buffer << "MOVE" << getNumber(pc);
				break;

			case RESERVE:
				buffer << "RESERVE";
				break;

			case JUMP:
				buffer << "JUMP" << offsets[getNumber(pc)];
				break;

			case JUMP_TRUE:
				buffer << "JUMP_TRUE" << offsets[getNumber(pc)];
				break;

			case JUMP_FALSE:
				buffer << "JUMP_TRUE" << offsets[getNumber(pc)];
				break;

			case METHOD:
				buffer << "METHOD" << getString(getNumber(pc)) << "(" << getNumber(pc) << ")";
				break;

			case EXIT:
				buffer << "EXIT";
				break;

			case PUSH_TRY:
				buffer << "PUSH_TRY" << offsets[getNumber(pc)];
				break;

			case POP_TRY:
				buffer << "POP_TRY";
				break;
		}

		buffer << std::endl;
	}

	buffer << std::endl;

	buffer << "bytecode: " << bytecode.size() << std::endl;
	buffer << "constants: " << constants.size() << std::endl;
	buffer << "strings: " << strings.size() << std::endl;
	buffer << "marks: " << marks.size() << std::endl;
	buffer << "offsets: " << offsets.size() << std::endl;

	return buffer.str();
}


//
//	OtByteCodeClass::getMeta
//

OtType OtByteCodeClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtByteCodeClass>("Code", OtInternalClass::getMeta());
	}

	return type;
}


//
//	OtByteCodeClass::create
//

OtByteCode OtByteCodeClass::create(OtSource source) {
	OtByteCode bytecode = std::make_shared<OtByteCodeClass>(source);
	bytecode->setType(getMeta());
	return bytecode;
}
