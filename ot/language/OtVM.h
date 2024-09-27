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

class OtVM : public OtPerThreadSingleton<OtVM> {
public:
	// constructor
	OtVM();

	// execute bytecode in the virtual machine
	OtObject execute(OtByteCode bytecode, size_t callingParameters=0);

	// call a member function on an object with arguments
	template<typename... ARGS>
	OtObject callMemberFunction(OtObject target, size_t member, ARGS... args) {
		const size_t count = sizeof...(ARGS) + 1;
		stack.push(target);
		(stack.push(args), ...);
		auto result = target->get(member)->operator () (count, stack.sp(count));
		stack.pop(count);
		return result;
	}

	template<typename... ARGS>
	OtObject callMemberFunction(OtObject target, const std::string& member, ARGS... args) {
		return callMemberFunction(target, OtIdentifier::create(member), std::forward<ARGS>(args)...);
	}

	inline OtObject callMemberFunction(OtObject target, OtObject member, size_t count, OtObject* args) {
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
	inline OtObject redirectMemberFunction(OtObject target, OtObject member, size_t count) {
		auto sp = stack.sp(count + 1);
		*sp = target;
		return member->operator () (count + 1, sp);
	}

	inline OtObject redirectMemberFunction(OtObject target, size_t member, size_t count) {
		auto sp = stack.sp(count + 1);
		*sp = target;
		return target->get(member)->operator () (count + 1, sp);
	}

	inline OtObject redirectMemberFunction(OtObject target, const std::string& member, size_t count) {
		return redirectMemberFunction(target, OtIdentifier::create(member), count);
	}

	// debugging functions
	inline void setStatementHook(std::function<void(OtByteCode, size_t)> hook) {
		statementHook = hook;
		callHook = hook != nullptr;
	}

	// get engine parameters
	static inline OtStack* getStack() { return &instance()->stack; }
	static inline OtClosure getClosure() { return instance()->stack.getClosure(); }
	static inline OtGlobal getGlobal() { return instance()->global; }
	static inline OtObject getNull() { return instance()->null; }

private:
	// VM properties
	OtStack stack;
	OtGlobal global;
	OtObject null;

	// debugging support
	std::function<void(OtByteCode, size_t)> statementHook;
	bool callHook = false;
};
