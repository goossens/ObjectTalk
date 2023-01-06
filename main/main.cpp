//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdlib>
#include <iostream>

#include <argparse/argparse.hpp>

#include "OtLibuv.h"
#include "OtModule.h"

#include "OtHttp.h"

#if defined(INCLUDE_GUI)
#include "OtGui.h"
#include "OtWorkspace.h"
#endif


//
//	ObjectTalk interpreter main function
//

int main(int argc, char* argv[]) {
	// parse all command line parameters
	argparse::ArgumentParser program(argv[0], "0.2");

	program.add_argument("-i", "--ide")
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
		std::_Exit(EXIT_FAILURE);
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

		// register HTTP module
		OtHttpClass::registerModule();

#if defined(INCLUDE_GUI)
		// register GUI module (if available on this platform)
		OtGuiClass::registerModule();
#endif

		// where any script specified?
		if (scripts.size() == 0) {
			// no, do we start the IDE workspace?
#if defined(INCLUDE_GUI)
			OtWorkspaceClass::instance()->run();

#else
			std::cerr << "No scripts specified" << std::endl << std::endl;
			std::cerr << program;
			std::_Exit(EXIT_FAILURE);
#endif


		} else {
#if defined(INCLUDE_GUI)
			if (program["--ide"] == true) {
				for (auto& script : scripts) {
					OtWorkspaceClass::instance()->openFile(script);
				}

				OtWorkspaceClass::instance()->run();

			} else {
#endif

				// run each script
				for (auto& script : scripts) {
					auto module = OtModuleClass::create();
					module->load(script);
					module->unsetAll();
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
		std::_Exit(EXIT_FAILURE);
	}

	return 0;
}
