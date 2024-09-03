//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtByteCodeFunction.h"
#include "OtClosure.h"
#include "OtException.h"
#include "OtFunction.h"


//
//	Sanity check
//

inline void OtCallbackValidate(OtObject& callback, size_t pars) {
	// sanity check
	if (callback->isKindOf("Function")) {
		auto parameters = OtFunction(callback)->getParameterCount();

		if (parameters != pars) {
			OtError("Callback must accept {} parameters, not {}", pars, parameters);
		}

	} else if (callback->isKindOf("ByteCodeFunction")) {
		auto parameters = OtByteCodeFunction(callback)->getParameterCount();

		if (parameters != pars) {
			OtError("Callback must accept {} parameters, not {}", pars, parameters);
		}

	} else if (callback->isKindOf("Closure")) {
		auto parameters = OtClosure(callback)->getParameterCount();

		if (parameters != pars) {
			OtError("Callback must accept {} parameters, not {}", pars, parameters);
		}

	} else {
		OtError("Expected a [Function], not a [{}]", callback->getType()->getName());
	}
}
