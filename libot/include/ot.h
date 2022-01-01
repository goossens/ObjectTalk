//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "ot/format.h"
#include "ot/exception.h"
#include "ot/numbers.h"
#include "ot/hash.h"
#include "ot/cache.h"
#include "ot/concurrentqueue.h"
#include "ot/libuv.h"

#include "ot/codepoint.h"
#include "ot/text.h"
#include "ot/source.h"
#include "ot/members.h"

#include "ot/type.h"
#include "ot/object.h"

#include "ot/primitive.h"
#include "ot/boolean.h"
#include "ot/integer.h"
#include "ot/real.h"
#include "ot/string.h"
#include "ot/function.h"

#include "ot/collection.h"
#include "ot/array.h"
#include "ot/dict.h"
#include "ot/set.h"

#include "ot/system.h"
#include "ot/io.h"
#include "ot/os.h"
#include "ot/fs.h"
#include "ot/path.h"
#include "ot/worker.h"

#include "ot/internal.h"
#include "ot/class.h"
#include "ot/closure.h"
#include "ot/iterator.h"
#include "ot/module.h"
#include "ot/global.h"
#include "ot/boundfunction.h"
#include "ot/bytecodefunction.h"

#include "ot/reference.h"
#include "ot/arrayreference.h"
#include "ot/capturereference.h"
#include "ot/classreference.h"
#include "ot/dictreference.h"
#include "ot/memberreference.h"
#include "ot/objectreference.h"
#include "ot/stackreference.h"
#include "ot/stringreference.h"

#include "ot/iterator.h"
#include "ot/arrayiterator.h"
#include "ot/pathiterator.h"
#include "ot/rangeiterator.h"
#include "ot/setiterator.h"
#include "ot/stringiterator.h"

#include "ot/registry.h"
#include "ot/singleton.h"
#include "ot/stack.h"
#include "ot/throw.h"

#include "ot/bytecode.h"
#include "ot/scanner.h"
#include "ot/compiler.h"

#include "ot/vm.h"
