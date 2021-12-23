//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <iostream>

#include "ot.h"


//
//	ObjectTalk interpreter main function
//

int main(int argc, char* argv[]) {
	try {
		// initialize libuv
		OtLibUv::init(argc, argv);

		// ensure we have some arguments
		if (argc == 1) {
			std::wcerr << argv[0] << ": usage: " << argv[0] << " script ..." << std::endl;
			exit(EXIT_FAILURE);
		}

		// run each script
		for (auto c = 1; c < argc; c++) {
			OtModuleClass::create(argv[c]);
		}

		// cleanup
		OtLibUv::end();

	} catch (const OtException& e) {
		// handle all failures
		std::wcerr << "Error: " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	return 0;
}
