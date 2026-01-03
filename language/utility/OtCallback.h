//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtByteCodeFunction.h"
#include "OtClosure.h"
#include "OtFunction.h"
#include "OtLog.h"


//
//	Sanity check
//

inline void OtCallbackValidate(OtObject& callback, size_t pars) {
	// sanity check
	if (callback.isKindOf<OtFunctionClass>()) {
		auto parameters = OtFunction(callback)->getParameterCount();

		if (parameters != pars) {
			OtLogError("Callback must accept {} parameters, not {}", pars, parameters);
		}

	} else if (callback.isKindOf<OtByteCodeFunctionClass>()) {
		auto parameters = OtByteCodeFunction(callback)->getParameterCount();

		if (parameters != pars) {
			OtLogError("Callback must accept {} parameters, not {}", pars, parameters);
		}

	} else if (callback.isKindOf<OtClosureClass>()) {
		auto parameters = OtClosure(callback)->getParameterCount();

		if (parameters != pars) {
			OtLogError("Callback must accept {} parameters, not {}", pars, parameters);
		}

	} else {
		OtLogError("Expected a [Function], not a [{}]", callback.getTypeName());
	}
}
