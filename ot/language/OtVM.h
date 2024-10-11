//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>

#include "OtObject.h"
#include "OtByteCode.h"
#include "OtClosure.h"
#include "OtGlobal.h"
#include "OtIdentifier.h"
#include "OtSingleton.h"
#include "OtStack.h"


//
//	The ObjectTalk Virtual Machine
//

class OtVM : OtPerThreadSingleton<OtVM> {
public:
	// execute bytecode in the virtual machine
	static inline OtObject execute(OtByteCode bytecode, size_t callingParameters=0) { return instance().executeByteCode(bytecode, callingParameters); }

	// call a member function on an object with arguments
	template<typename... ARGS>
	static inline OtObject callMemberFunction(OtObject target, OtID member, ARGS... args) {
		auto& stack = instance().stack;
		const size_t count = sizeof...(ARGS) + 1;
		stack.push(target);
		(stack.push(args), ...);
		auto result = target->get(member)->operator () (count, stack.sp(count));
		stack.pop(count);
		return result;
	}

	template<typename... ARGS>
	static inline OtObject callMemberFunction(OtObject target, const std::string& member, ARGS... args) {
		return callMemberFunction(target, OtIdentifier::create(member), std::forward<ARGS>(args)...);
	}

	static inline OtObject callMemberFunction(OtObject target, OtObject member, size_t count, OtObject* args) {
		auto& stack = instance().stack;
		stack.push(target);

		for (auto c = 0; c < count; c ++) {
			(stack.push(args[c]));
		}

		count++;
		auto result = member->operator () (count, stack.sp(count));
		stack.pop(count);
		return result;
	}

	// redirect member function to a new target
	static inline OtObject redirectMemberFunction(OtObject target, OtObject member, size_t count) {
		auto sp = instance().stack.sp(count + 1);
		*sp = target;
		return member->operator () (count + 1, sp);
	}

	static inline OtObject redirectMemberFunction(OtObject target, OtID member, size_t count) {
		auto sp = instance().stack.sp(count + 1);
		*sp = target;
		return target->get(member)->operator () (count + 1, sp);
	}

	static inline OtObject redirectMemberFunction(OtObject target, const std::string& member, size_t count) {
		return redirectMemberFunction(target, OtIdentifier::create(member), count);
	}

	// debugging functions
	static inline void setStatementHook(std::function<void()> hook) {
		instance().statementHook = hook;
		instance().callHook = hook != nullptr;
	}

	// get engine parameters
	static inline OtStack* getStack() { return &instance().stack; }
	static inline OtGlobal getGlobal() { return instance().global; }
	static inline OtObject getNull() { return instance().null; }

	static inline OtClosure getClosure() { return instance().stack.getClosure(); }
	static inline OtStackFrame& getStackFrame() { return instance().stack.getFrame(); }

private:
	// execute bytecode in the virtual machine
	OtObject executeByteCode(OtByteCode bytecode, size_t callingParameters);

	// VM properties
	OtStack stack;
	OtGlobal global = OtGlobal::create();
	OtObject null = OtObject::create();

	// debugging support
	std::function<void()> statementHook;
	bool callHook = false;
};
