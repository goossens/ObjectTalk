//	ObjectTalk Scripting Language
//	Copyright 1993-2020 Johan A. Goossens
//
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.


#pragma once


//
//	Include files
//

#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <codecvt>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "OtFormat.h"
#include "OtException.h"
#include "OtText.h"

#include "OtDefs.h"
#include "OtType.h"
#include "OtObject.h"

#include "OtPrimitive.h"
#include "OtFunction.h"
#include "OtBoolean.h"
#include "OtInteger.h"
#include "OtReal.h"
#include "OtString.h"

#include "OtInternal.h"
#include "OtClass.h"

#include "OtCollection.h"
#include "OtArray.h"
#include "OtDict.h"

#include "OtCode.h"
#include "OtScanner.h"
#include "OtCompiler.h"
#include "OtObjectTalk.h"

#include "OtTypeImpl.h"
#include "OtObjectImpl.h"
