//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bytecodefunction.h"
#include "exception.h"
#include "function.h"


//
//	Sanity check
//

inline void OtCallbackValidate(OtObject callback, size_t pars) {
	// sanity check
	if (callback->isKindOf("Function")) {
		auto parameters = callback->cast<OtFunctionClass>()->getParameterCount();

		if (parameters != pars) {
			OtExcept("Callback must accept %d parameters, not %d", pars, parameters);
		}

	} else if (callback->isKindOf("ByteCodeFunction")) {
		auto parameters = callback->cast<OtByteCodeFunctionClass>()->getParameterCount();

		if (parameters != pars) {
			OtExcept("Callback must accept %d parameters, not %d", pars, parameters);
		}

	} else {
		OtExcept("Expected a [Function] object, not a [%s]", callback->getType()->getName().c_str());
	}
}