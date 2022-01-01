//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <string>

#include "ot/format.h"
#include "ot/exception.h"
#include "ot/string.h"
#include "ot/function.h"
#include "ot/vm.h"


//
//	OtTryCatch
//

class OtTryCatch {
public:
	OtTryCatch(size_t p, OtStackState s) : pc(p), stack(s) {}
	size_t pc;
	OtStackState stack;
};


//
//	OtVM::OtVM
//

OtVM::OtVM() {
	// create stack
	stack = OtStackClass::create();

	// create globals
	global = OtGlobalClass::create();

	// create null object
	null = OtObjectClass::create();
}


//
//	OtVM::exec
//

OtObject OtVM::exec(OtByteCode bytecode, size_t callingParameters) {
	// try/catch stack
	std::vector<OtTryCatch> tryCatch;

	// save the current stack state (so we can restore it in case of an uncaught exception)
	OtStackState state = stack->getState();

	// local variables
	size_t sp = stack->size();
	size_t pc = 0;
	size_t end = bytecode->size();
	size_t mark;

	// open a new stack frame
	stack->openFrame(callingParameters);

	// execute all instructions
	while (pc < end) {
		try {
			switch (bytecode->getOpcode(pc)) {
				case OtByteCodeClass::DEBUG:
					OT_DEBUG(bytecode->disassemble());
					OT_DEBUG(OtFormat("PC: %ld\n", pc));
					OT_DEBUG(stack->debug());
					break;

				case OtByteCodeClass::MARK:
					mark = bytecode->getNumber(pc);
					break;

				case OtByteCodeClass::PUSH:
					stack->push(bytecode->getConstant(bytecode->getNumber(pc)));
					break;

				case OtByteCodeClass::POP:
					stack->pop();
					break;

				case OtByteCodeClass::POP_COUNT:
					stack->pop(bytecode->getNumber(pc));
					break;

				case OtByteCodeClass::DUP:
					stack->dup();
					break;

				case OtByteCodeClass::SWAP:
					stack->swap();
					break;

				case OtByteCodeClass::MOVE:
					stack->move(bytecode->getNumber(pc));
					break;

				case OtByteCodeClass::RESERVE:
					stack->reserve();
					break;

				case OtByteCodeClass::JUMP:
					pc = bytecode->getOffset(bytecode->getNumber(pc));
					break;

				case OtByteCodeClass::JUMP_TRUE: {
					auto jump = bytecode->getOffset(bytecode->getNumber(pc));
					auto value = stack->pop();

					if (value->operator bool()) {
						pc = jump;
					}

					break;
				}

				case OtByteCodeClass::JUMP_FALSE: {
					auto jump = bytecode->getOffset(bytecode->getNumber(pc));
					auto value = stack->pop();

					if (!value->operator bool()) {
						pc = jump;
					}

					break;
				}

				case OtByteCodeClass::METHOD: {
					// get method name
					auto method = bytecode->getString(bytecode->getNumber(pc));

					// get number of calling parameters
					auto count = bytecode->getNumber(pc);

					// get a pointer to the calling parameters and target object
					auto parameters = stack->sp(count + 1);

					// sanity check
					OT_ASSERT(parameters[0]);

					// call method
					auto result = parameters[0]->get(method)->operator () (count + 1, parameters);

					// clean up stack and put result back on stack
					stack->pop(count + 1);
					stack->push(result);
					break;
				}

				case OtByteCodeClass::EXIT:
					// exit instructions
					pc = end;
					break;

				case OtByteCodeClass::PUSH_TRY:
					// start a new try/catch cycle
					tryCatch.push_back(OtTryCatch(bytecode->getOffset(bytecode->getNumber(pc)), stack->getState()));
					break;

				case OtByteCodeClass::POP_TRY:
					// start a new try/catch cycle
					tryCatch.pop_back();
					break;
			}

		} catch (const OtException& e) {
			// do we have an exception handler
			if (tryCatch.size()) {
				// yes, use it
				OtTryCatch trycatch = tryCatch.back();
				tryCatch.pop_back();

				// restore program counter and stack
				pc = trycatch.pc;
				stack->restoreState(trycatch.stack);

				// put exception on stack
				stack->push(OtStringClass::create(e.what()));

			} else {
				// no exception handler, restore the stack state
				stack->restoreState(state);

				// get source code
				OtSource source = bytecode->getSource();

				// find start and end marker
				size_t startMarker = bytecode->getMark(mark);
				size_t endMark = bytecode->getNextMark(mark);

				// get offending line(s)
				auto line = source->getLineNumber(startMarker);
				auto lines = source->getLines(startMarker, endMark);

				// format nicely
				std::string statement;

				OtText::splitIterator(lines.data(), lines.data() + lines.size(), '\n', [&](const char *b, const char *e) {
					std::string text(b, e - b);

					if (statement.size()) {
						statement += '\n';
					}

					statement += OtFormat("Line %ld: %s", line++, text.c_str());
				});

				// throw the exception
				OtExcept("%s\nModule: %s\n%s", e.what(), source->getModule().c_str(), statement.c_str());
			}
		}
	}

	// get code result
	auto result = stack->pop();

	// close the stack frame
	stack->closeFrame();

	// sanity check
	OT_ASSERT(tryCatch.size() == 0);
	OT_ASSERT(stack->size() == sp);

	// result xecution result
	return result;
}
