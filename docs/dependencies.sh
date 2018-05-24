#!/bin/bash

#	ObjectTalk Scripting Language
#	Copyright 1993-2018 Johan A. Goossens
#
#	Licensed under the Apache License, Version 2.0 (the "License");
#	you may not use this file except in compliance with the License.
#	You may obtain a copy of the License at
#
#		http:#www.apache.org/licenses/LICENSE-2.0
#
#	Unless required by applicable law or agreed to in writing, software
#	distributed under the License is distributed on an "AS IS" BASIS,
#	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#	See the License for the specific language governing permissions and
#	limitations under the License.

dot -Tpng -oFileDependencies.png <<END
digraph d {
"OtMain.cpp" -> "OtObjectTalk.h"

"OtObjectTalk.h" -> "OtClasses.h"
"OtObjectTalk.h" -> "OtScanner.h"

"OtClasses.cpp" -> "OtClasses.h"

"OtClasses.h" -> "OtNucleus.h"
"OtClasses.h" -> "OtPrimitives.h"
"OtClasses.h" -> "OtCollections.h"
"OtClasses.h" -> "OtIO.h"
"OtClasses.h" -> "OtInternal.h"

"OtScanner.h" -> "OtException.h"

"OtNucleus.h" -> "OtException.h"

"OtException.h" -> "OtFormat.h"
}
END
