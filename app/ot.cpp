//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <iostream>

#include <argparse/argparse.hpp>

#include "ot.h"

#if defined(INCLUDE_GUI)
#include "guimodule.h"
#include "workspace.h"
#endif


//
//	ObjectTalk interpreter main function
//

int main(int argc, char* argv[]) {
	// parse all command line parameters
	argparse::ArgumentParser program(argv[0], "0.2");

	program.add_argument("-e", "--editor")
		.help("edit code instead of running it")
		.default_value(false)
		.implicit_value(true);

	program.add_argument("scripts")
		.help("scripts to execute")
		.remaining();

	try {
		program.parse_args(argc, argv);

	} catch (const std::runtime_error& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		exit(EXIT_FAILURE);
	}

	// get all the script file names
	std::vector<std::string> scripts;

	try {
		scripts = program.get<std::vector<std::string>>("scripts");

	} catch (std::logic_error& e) {
	}

	try {
		// initialize libuv
		OtLibUv::init(argc, argv);

#if defined(INCLUDE_GUI)
		// initialize GUI module (if available on this platform)
		OtGuiModuleInit();
#endif

		// where any script specified?
		if (scripts.size() == 0) {
			// no, do we start the editor?
#if defined(INCLUDE_GUI)
			OtWorkspaceClass::instance()->init();
			OtWorkspaceClass::instance()->newFile();
			OtWorkspaceClass::instance()->run();

#else
			std::cerr << "No scripts specified" << std::endl << std::endl;
			std::cerr << program;
			exit(EXIT_FAILURE);
#endif


		} else {
#if defined(INCLUDE_GUI)
			if (program["--editor"] == true) {
				OtWorkspaceClass::instance()->init();

				for (auto& script : scripts) {
					OtWorkspaceClass::instance()->openFile(script);
				}

				OtWorkspaceClass::instance()->run();

			} else {
#endif

				// run each script
				for (auto& script : scripts) {
					OtModuleClass::create(script);
				}
#if defined(INCLUDE_GUI)
			}
#endif
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
