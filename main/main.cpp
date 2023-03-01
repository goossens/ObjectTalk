//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdlib>
#include <filesystem>
#include <iostream>

#include <argparse/argparse.hpp>

#include "OtCompiler.h"
#include "OtLibuv.h"
#include "OtModule.h"

#include "OtHttp.h"

#if defined(INCLUDE_GUI)
#include "OtGui.h"
#include "OtWorkspace.h"
#include "OtSceneApp.h"
#endif


//
//	ObjectTalk main function
//

int main(int argc, char* argv[]) {
	// parse all command line parameters
	argparse::ArgumentParser program(argv[0], "0.2");

	program.add_argument("-d", "--disassemble")
		.help("disassemble bytecode")
		.default_value(false)
		.implicit_value(true);

	program.add_argument("-i", "--ide")
		.help("edit files instead of running them")
		.default_value(false)
		.implicit_value(true);

	program.add_argument("files")
		.help("files to process")
		.remaining();

	try {
		program.parse_args(argc, argv);

	} catch (const std::runtime_error& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		std::_Exit(EXIT_FAILURE);
	}

	// get all the target filenames
	std::vector<std::string> files;

	try {
		files = program.get<std::vector<std::string>>("files");

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

		// where any files specified?
		if (files.size() == 0) {
			// no, do we start an IDE workspace?
#if defined(INCLUDE_GUI)
			OtWorkspaceClass::instance()->run();

#else
			std::cerr << "No files specified" << std::endl << std::endl;
			std::cerr << program;
			std::_Exit(EXIT_FAILURE);
#endif


		} else {
#if defined(INCLUDE_GUI)
			if (program["--ide"] == true) {
				auto workspace = OtWorkspaceClass::instance();

				for (auto& file : files) {
					workspace->openFile(file);
				}

				workspace->run();

			} else {
#endif
				// we can only handle one file
				if (files.size() != 1) {
					std::wcerr << "Error:you can only run one file, you specified [%d]" << files.size() << std::endl;
					std::_Exit(EXIT_FAILURE);
				}

				// run the file
				auto file = std::filesystem::path(files[0]);
				auto extension = file.extension();

				// execute by type (based by the file extension)
				if (extension == ".ot" || extension == "") {
					// handle a script file
					if (program["--disassemble"] == true) {
						OtCompiler compiler;
						auto object = OtObject::create();
						auto bytecode = compiler.compileFile(file, object);
						std::cout << bytecode->disassemble() << std::endl;

					} else {
						auto module = OtModule::create();
						module->load(file);
						module->unsetAll();
					}

#if defined(INCLUDE_GUI)
				} else if (extension == ".ots") {
					// handle a scene file
					OtSceneApp app;
					app.run(std::filesystem::path(file));
#endif

				} else {
					std::wcerr << "Error: can't execute file with extension [%s]" << extension.c_str() << std::endl;
					std::_Exit(EXIT_FAILURE);
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
