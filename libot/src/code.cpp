//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "ot/format.h"
#include "ot/exception.h"
#include "ot/text.h"
#include "ot/string.h"
#include "ot/code.h"


//
//	OtTryCatch
//

class OtTryCatch {
public:
	OtTryCatch(size_t p, size_t s) : pc(p), sp(s) {}
	size_t pc;
	size_t sp;
};


//
//
//	OtCodeClass::operator ()
//

OtObject OtCodeClass::operator ()(OtContext context) {
	// stack
	std::vector<OtObject> stack;

	// try/catch stack
	std::vector<OtTryCatch> tryCatch;

	// local variables
	size_t end = bytecode.size();
	size_t pc = 0;
	size_t mark;

	// execute all instructions
	while (pc < end) {
		try {
			switch (bytecode[pc++]) {
				case MARK:
					mark = getNumber(&pc);
					break;

				case PUSH:
					stack.push_back(getConstant(&pc));
					break;

				case POP:
					stack.pop_back();
					break;

				case POP_COUNT:
					stack.resize(stack.size() - getNumber(&pc));
					break;

				case DUP:
					stack.push_back(stack.back());
					break;

				case SWAP:
					std::swap(stack[stack.size() - 1], stack[stack.size() - 2]);
					break;

				case JUMP:
					pc = offsets[getNumber(&pc)];
					break;

				case JUMP_TRUE: {
					auto jump = offsets[getNumber(&pc)];
					auto value = stack.back();
					stack.pop_back();

					if (value->operator bool()) {
						pc = jump;
					}

					break;
				}

				case JUMP_FALSE: {
					auto jump = offsets[getNumber(&pc)];
					auto value = stack.back();
					stack.pop_back();

					if (!value->operator bool()) {
						pc = jump;
					}

					break;
				}

				case METHOD: {
					// get method and target object
					auto method = getString(&pc);
					auto cnt = getNumber(&pc) + 1;
					auto sp = &stack[stack.size() - cnt];

					// sanity check
					if (!sp[0]) {
						OtExcept("You can't call a method on the null object");
					}

					// call method
					auto value = sp[0]->get(method)->operator ()(context, cnt, sp);

					// clean up stack
					stack.resize(stack.size() - cnt);
					stack.push_back(value);
					break;
				}

				case EXIT:
					// exit instructions
					pc = end;
					break;

				case PUSH_TRY:
					// start a new try/catch cycle
					tryCatch.push_back(OtTryCatch(offsets[getNumber(&pc)], stack.size()));
					break;

				case POP_TRY:
					// start a new try/catch cycle
					tryCatch.pop_back();
					break;

				case PUSH_CONTEXT: {
					// push current context
					auto ctx = stack.back()->cast<OtContextClass>();
					stack.pop_back();
					ctx->setParent(context);
					context = ctx;
					break;
				}

				case POP_CONTEXT: {
					// return to previous context
					auto ctx = context->getParent();
					context->setParent(nullptr);
					context = ctx;
					break;
				}
			}

		} catch (const OtException& e) {
			// do we have an exception handler
			if (tryCatch.size()) {
				// yes, use it
				OtTryCatch trycatch = tryCatch.back();
				tryCatch.pop_back();

				// restore program counter and stack
				pc = trycatch.pc;
				stack.resize(trycatch.sp);

				// put exception on stack
				stack.push_back(OtStringClass::create(e.what()));

			} else {
				// find start and end marker
				size_t startMarker = marks[mark];
				size_t endMark = source->size();

				if (mark < marks.size()) {
					endMark = marks[mark + 1];
				}

				// get offending line(s)
				auto line = source->getLineNumber(startMarker);
				auto lines = source->getLines(startMarker, endMark);

				// format nicely
				std::string statement;

				OtTextSplitIterator(lines.data(), lines.data() + lines.size(), ';', [&](const char *b, const char *e) {
					std::string text(b, e - b);

					if (statement.size()) {
						statement += '\n';
					}

					statement += OtFormat("Line %ld: %s", line++, text.c_str());
				});

				OtExcept("%s\nModule: %s\n%s", e.what(), source->getModule().c_str(), statement.c_str());
			}
		}
	}

	OT_ASSERT(tryCatch.size() == 0);
	OT_ASSERT(stack.size() == 1);
	return stack.back();
}


//
//	OtCodeClass::disassemble
//

std::string OtCodeClass::disassemble() {
	std::stringstream buffer;
	size_t pc = 0;
	size_t end = bytecode.size();

	while (pc < end) {
		buffer << std::right;
		buffer << std::setw(4) << std::setfill('0') << pc << std::setfill(' ') << " ";
		buffer << std::left << std::setw(16);

		switch (bytecode[pc++]) {
			case MARK: {
				auto n = getNumber(&pc);
				buffer << "MARK" << n << ", " << marks[n];
				break;
			}

			case PUSH: {
				auto n = getNumber(&pc);
				auto klass = constants[n] ? constants[n]->getType()->getName() : "null";
				buffer << "PUSH" << n << ", " << klass;
				break;
			}

			case POP:
				buffer << "POP";
				break;

			case POP_COUNT:
				buffer << "POP_COUNT" << getNumber(&pc);
				break;

			case DUP:
				buffer << "DUP";
				break;

			case SWAP:
				buffer << "SWAP";
				break;

			case JUMP:
				buffer << "JUMP" << offsets[getNumber(&pc)];
				break;

			case JUMP_TRUE:
				buffer << "JUMP_TRUE" << offsets[getNumber(&pc)];
				break;

			case JUMP_FALSE:
				buffer << "JUMP_TRUE" << offsets[getNumber(&pc)];
				break;

			case METHOD:
				buffer << "METHOD" << getString(&pc) << ", " << getNumber(&pc);
				break;

			case EXIT:
				buffer << "EXIT";
				break;

			case PUSH_TRY:
				buffer << "PUSH_TRY" << offsets[getNumber(&pc)];
				break;

			case POP_TRY:
				buffer << "POP_TRY";
				break;

			case PUSH_CONTEXT:
				buffer << "PUSH_CONTEXT";
				break;

			case POP_CONTEXT:
				buffer << "POP_CONTEXT";
				break;
 		}

		buffer << std::endl;
	}

	return buffer.str();
}


//
//	OtCodeClass::getMeta
//

OtType OtCodeClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtCodeClass>("Code", OtInternalClass::getMeta());
	}

	return type;
}


//
//	OtCodeClass::create
//

OtCode OtCodeClass::create(OtSource source) {
	OtCode code = std::make_shared<OtCodeClass>(source);
	code->setType(getMeta());
	return code;
}
