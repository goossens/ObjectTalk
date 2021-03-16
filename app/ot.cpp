//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <iostream>
#include <uv.h>
#include "ot.h"


//
//	ObjectTalk interpreter main function
//

int main(int argc, char* argv[]) {
	// ensure we have some arguments
	if (argc == 1) {
		std::wcerr << argv[0] << ": usage: " << argv[0] << " script ..." << std::endl;
		exit(EXIT_FAILURE);
	}

	// initialize libuv
	uv_setup_args(argc, argv);

	// create globale context
	OtGlobal global = OtGlobalClass::create();

	try {
		// run each script in the global context
		for (auto c = 1; c < argc; c++) {
			OtModule module = OtModuleClass::create();
			module->run(argv[c], global);
		}

	} catch (const OtException& e) {
		// handle all failures
		std::wcerr << argv[0] <<": error: " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	return 0;
}
