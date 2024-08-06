//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <string>

#include "fmt/format.h"

#include "OtAssert.h"
#include "OtClass.h"
#include "OtException.h"
#include "OtFunction.h"
#include "OtLog.h"
#include "OtMemberReference.h"
#include "OtSymbol.h"
#include "OtString.h"
#include "OtVM.h"


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
	// create globals
	global = OtGlobal::create();

	// create null object
	null = OtObject::create();
}


//
//	OtVM::execute
//

OtObject OtVM::execute(OtByteCode bytecode, size_t callingParameters) {
	// try/catch stack
	std::vector<OtTryCatch> tryCatch;

	// save the current stack state (so we can restore it in case of an uncaught exception)
	OtStackState state = stack.getState();

	// local variables
	size_t sp = stack.size();
	size_t pc = 0;
	size_t end = bytecode->size();
	size_t mark;

	// open a new stack frame
	stack.openFrame(callingParameters);

	// execute all instructions
	while (pc < end) {
		try {
			switch (bytecode->getOpcode(pc)) {
				case OtByteCodeClass::debugOpcode:
					OtLogInfo(bytecode->disassemble());
					OtLogInfo("PC: {}\n", pc);
					OtLogInfo(stack.debug());
					break;

				case OtByteCodeClass::markOpcode:
					mark = bytecode->getNumber(pc);
					break;

				case OtByteCodeClass::pushOpcode:
					stack.push(bytecode->getConstant(bytecode->getNumber(pc)));
					break;

				case OtByteCodeClass::popOpcode:
					stack.pop();
					break;

				case OtByteCodeClass::popCountOpcode:
					stack.pop(bytecode->getNumber(pc));
					break;

				case OtByteCodeClass::dupOpcode:
					stack.dup();
					break;

				case OtByteCodeClass::swapOpcode:
					stack.swap();
					break;

				case OtByteCodeClass::moveOpcode:
					stack.move(bytecode->getNumber(pc));
					break;

				case OtByteCodeClass::reserveOpcode:
					stack.reserve();
					break;

				case OtByteCodeClass::jumpOpcode:
					pc = bytecode->getOffset(bytecode->getNumber(pc));
					break;

				case OtByteCodeClass::jumpTrueOpcode: {
					auto jump = bytecode->getOffset(bytecode->getNumber(pc));
					auto value = stack.pop();

					if (value->operator bool()) {
						pc = jump;
					}

					break;
				}

				case OtByteCodeClass::jumpFalseOpcode: {
					auto jump = bytecode->getOffset(bytecode->getNumber(pc));
					auto value = stack.pop();

					if (!value->operator bool()) {
						pc = jump;
					}

					break;
				}

				case OtByteCodeClass::memberOpcode: {
					// create a member reference
					auto object = stack.pop();
					auto member = bytecode->getNumber(pc);
					auto reference = OtMemberReference::create(object, member);
					stack.push(reference);
					break;
				}

				case OtByteCodeClass::superOpcode: {
					auto cls = OtClass(stack.pop());
					auto member = bytecode->getNumber(pc);
					auto result = cls->getSuper(member);
					stack.push(result);
					break;
				}

				case OtByteCodeClass::methodOpcode: {
					// get method and number of calling parameters
					auto method = bytecode->getNumber(pc);
					auto count = bytecode->getNumber(pc);

					// get a pointer to the calling parameters and target object
					auto parameters = stack.sp(count + 1);

					// sanity check
					if (!parameters[0]) {
						OtLogFatal("Internal error: can't call method [{}] with [{}] parameters on nullptr", OtSymbol::name(method), count);
					}

					// call method
					auto result = parameters[0]->get(method)->operator () (count + 1, parameters);

					// remove arguments from stack and put result back on it
					stack.pop(count + 1);
					stack.push(result);
					break;
				}

				case OtByteCodeClass::exitOpcode:
					// exit instructions
					pc = end;
					break;

				case OtByteCodeClass::pushTryOpcode:
					// start a new try/catch cycle
					tryCatch.push_back(OtTryCatch(bytecode->getOffset(bytecode->getNumber(pc)), stack.getState()));
					break;

				case OtByteCodeClass::popTryOpcode:
					// start a new try/catch cycle
					tryCatch.pop_back();
					break;

				case OtByteCodeClass::pushStackOpcode: {
					auto slot = bytecode->getNumber(pc);
					auto object  = stack.getFrameItem(OtStackItem(0, slot));
					stack.push(object);
					break;
				}

				case OtByteCodeClass::pushObjectMemberOpcode: {
					auto object = bytecode->getConstant(bytecode->getNumber(pc));
					auto member = bytecode->getNumber(pc);
					auto resolvedMember = OtMemberReferenceClass::resolveMember(object, member);
					stack.push(resolvedMember);
					break;
				}
				case OtByteCodeClass::pushMemberOpcode: {
					auto object = stack.pop();
					auto member = bytecode->getNumber(pc);
					auto resolvedMember = OtMemberReferenceClass::resolveMember(object, member);
					stack.push(resolvedMember);
					break;
				}

				case OtByteCodeClass::assignStackOpcode: {
					auto slot = bytecode->getNumber(pc);
					auto value = stack.back();
					stack.setFrameItem(OtStackItem(0, slot), value);
					break;
				}

				case OtByteCodeClass::assignMemberOpcode: {
					auto object = bytecode->getConstant(bytecode->getNumber(pc));
					auto member = bytecode->getNumber(pc);
					auto value = stack.back();
					object->set(member, value);
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
				stack.restoreState(trycatch.stack);

				// put exception on stack
				auto message = OtString::create(e.what());
				stack.push(message);

			} else {
				// no exception handler, restore the stack state
				stack.restoreState(state);

				// get source code
				OtSource source = bytecode->getSource();

				// find start and end marker
				size_t startMarker = bytecode->getMark(mark);
				size_t endMarker = bytecode->getNextMark(mark);

				// get offending line(s)
				auto line = source->getLineNumber(startMarker);
				auto lines = source->getLines(startMarker, endMarker);

				// format nicely
				std::string statement;

				OtText::splitIterator(lines.data(), lines.data() + lines.size(), '\n', [&](const char* b, const char* e) {
					std::string text(b, e - b);

					if (statement.size()) {
						statement += '\n';
					}

					statement += fmt::format("Line {}: {}", line++, text);
				});

				// format long message
				auto fullMessage = fmt::format(
					"{}\nModule: {}\n{}",
					e.what(),
					source->getModule(),
					statement);

				// throw exception
				if (e.getLineNumber()) {
					throw OtException(
						e.getModule(),
						e.getLineNumber(),
						e.getStart(),
						e.getEnd(),
						e.what(),
						fullMessage);

				} else {
					throw OtException(
						source->getModule(),
						source->getLineNumber(startMarker),
						startMarker,
						endMarker,
						e.what(),
						fullMessage);
				}
			}
		}
	}

	// get code result
	auto result = stack.pop();

	// close the stack frame
	stack.closeFrame();

	// sanity check
	OtAssert(tryCatch.size() == 0);
	OtAssert(stack.size() == sp);

	// return execution result
	return result;
}
