//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#if defined(WINVER)
#else
#include <dlfcn.h>
#endif

#include <algorithm>
#include <cctype>
#include <cmath>
#include <codecvt>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <random>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <whereami/whereami.h>

#include "OtFormat.h"
#include "OtException.h"
#include "OtCodePoint.h"
#include "OtText.h"

#include "OtDefs.h"
#include "OtType.h"
#include "OtObject.h"
#include "OtInternal.h"

#include "OtPrimitive.h"
#include "OtFunction.h"
#include "OtBoolean.h"
#include "OtInteger.h"
#include "OtReal.h"
#include "OtString.h"

#include "OtClass.h"

#include "OtCollection.h"
#include "OtArray.h"
#include "OtDict.h"

#include "OtCode.h"
#include "OtScanner.h"
#include "OtCompiler.h"
#include "OtModule.h"
#include "OtGlobal.h"

#include "OtTypeImpl.h"
#include "OtObjectImpl.h"
