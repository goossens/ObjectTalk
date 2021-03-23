//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <vector>
#include <algorithm>

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
//	OtCodeClass::operator ()
//

OtObject OtCodeClass::operator ()(OtContext context) {
	// stack
	std::vector<OtObject> stack;

	// try/catch stack
	std::vector<OtTryCatch> tryCatch;

	// local variables
	size_t end = size();
	size_t pc = 0;
	size_t mark;

	// execute all instructions
	while (pc < end) {
		try {
			switch (at(pc).opcode) {
				case OtInstruction::MARK:
					mark = at(pc).integer;
					break;

				case OtInstruction::PUSH:
					stack.push_back(at(pc).value);
					break;

				case OtInstruction::POP:
					stack.pop_back();
					break;

				case OtInstruction::POP_COUNT:
					stack.resize(stack.size() - at(pc).integer);
					break;

				case OtInstruction::DUP:
					stack.push_back(stack.back());
					break;

				case OtInstruction::SWAP:
					std::swap(stack[stack.size() - 1], stack[stack.size() - 2]);
					break;

				case OtInstruction::JUMP:
					pc = at(pc).integer - 1;
					break;

				case OtInstruction::JUMP_TRUE: {
					auto value = stack.back();
					stack.pop_back();

					if (value->operator bool()) {
						pc = at(pc).integer - 1;
					}

					break;
				}

				case OtInstruction::JUMP_FALSE: {
					auto value = stack.back();
					stack.pop_back();

					if (!value->operator bool()) {
						pc = at(pc).integer - 1;
					}

					break;
				}

				case OtInstruction::METHOD: {
					// get target object and call method
					auto cnt = at(pc).integer + 1;
					auto sp = &stack[stack.size() - cnt];

					// sanity check
					if (!sp[0]) {
						OtExcept("You can't call a method on the null object");
					}

					// call method
					auto method =at(pc).string;
					auto value = sp[0]->get(at(pc).string)->operator ()(context, cnt, sp);

					// clean up stack
					stack.resize(stack.size() - cnt);
					stack.push_back(value);
					break;
				}

				case OtInstruction::EXIT:
					// exit instructions
					pc = end;
					break;

				case OtInstruction::PUSH_TRY:
					// start a new try/catch cycle
					tryCatch.push_back(OtTryCatch(at(pc).integer, stack.size()));
					break;

				case OtInstruction::POP_TRY:
					// start a new try/catch cycle
					tryCatch.pop_back();
					break;

				case OtInstruction::PUSH_CONTEXT: {
					// push current context
					auto ctx = stack.back()->cast<OtContextClass>();
					stack.pop_back();
					ctx->setParent(context);
					context = ctx;
					break;
				}

				case OtInstruction::POP_CONTEXT: {
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
				pc = trycatch.pc - 1;
				stack.resize(trycatch.sp);

				// put exception on stack
				stack.push_back(OtStringClass::create(e.what()));

			} else {
				// find next marker
				size_t nextMark = source->size();
				bool found = false;

				for (auto i = pc; !found && i < end; i++) {
					if (at(i).opcode == OtInstruction::MARK) {
						nextMark = at(i).integer;
						found = true;
					}
				}

				// get text of offending line(s)
				auto line = source->getLineNumber(mark);
				auto lines = source->getLines(mark, nextMark);

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

		pc++;
	}

	OT_ASSERT(tryCatch.size() == 0);
	OT_ASSERT(stack.size() == 1);
	return stack.back();
}

OtCode OtCodeClass::create(OtSource source) {
	return std::make_shared<OtCodeClass>(source);
}
