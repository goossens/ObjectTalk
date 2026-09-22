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

inline void OtCallbackValidate(const OtObject& callback, size_t pars) {
	// sanity check
	if (callback.isKindOf<OtFunctionClass>()) {
		const auto parameters = OtFunction(callback)->getParameterCount();

		if (parameters != pars) {
			OtLogError("Callback must accept {} parameters, not {}", pars, parameters);
		}

	} else if (callback.isKindOf<OtByteCodeFunctionClass>()) {
		const auto parameters = OtByteCodeFunction(callback)->getParameterCount();

		if (parameters != pars) {
			OtLogError("Callback must accept {} parameters, not {}", pars, parameters);
		}

	} else if (callback.isKindOf<OtClosureClass>()) {
		const auto parameters = OtClosure(callback)->getParameterCount();

		if (parameters != pars) {
			OtLogError("Callback must accept {} parameters, not {}", pars, parameters);
		}

	} else {
		OtLogError("Expected a [Function], not a [{}]", callback.getTypeName());
	}
}
