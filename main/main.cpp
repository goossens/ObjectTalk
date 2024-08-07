//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <string>

#include <argparse/argparse.hpp>

#include "OtCompiler.h"
#include "OtLibuv.h"
#include "OtLog.h"
#include "OtPathTools.h"
#include "OtModule.h"

#if defined(INCLUDE_GUI)
#include "OtFramework.h"
#include "OtSceneApp.h"
#include "OtWorkspace.h"
#endif


//
//	ObjectTalk main function
//

int main(int argc, char* argv[]) {
	// parse all command line parameters
	argparse::ArgumentParser program(argv[0], "0.2");

	program.add_argument("-d", "--debug")
		.help("run in debug mode")
		.default_value(false)
		.implicit_value(true);

	program.add_argument("-i", "--ide")
		.help("edit files instead of running them")
		.default_value(false)
		.implicit_value(true);

	program.add_argument("-c", "--child")
		.help("run as an IDE child process")
		.default_value(false)
		.implicit_value(true);

	program.add_argument("files")
		.help("files to process")
		.remaining();

	try {
		program.parse_args(argc, argv);

	} catch (const std::runtime_error& err) {
		OtLogFatal(err.what());
	}

	// get all the target filenames
	std::vector<std::string> files;

	try {
		files = program.get<std::vector<std::string>>("files");

	} catch (std::logic_error& e) {
	}

	// configure logging engine
	OtLogger::instance()->setSubprocessMode(program["--child"] == true);

	// configure ObjectTalk compiler
	OtCompiler::setDebug(program["--debug"] == true || program["--child"] == true);

	try {
		// initialize libuv
		OtLibUv::init(argc, argv);

		// where any files specified?
		if (files.size() == 0) {
			// no, do we start an IDE workspace?
#if defined(INCLUDE_GUI)
			OtFramework framework;
			OtWorkspace workspace;
			framework.run(&workspace);

#else
	OtLogFatal("No files specified");
#endif


		} else {
#if defined(INCLUDE_GUI)
			if (program["--ide"] == true) {
				OtFramework framework;
				OtWorkspace workspace;

				for (auto& file : files) {
					workspace.openFile(file, OtEditor::inTab);
				}

				framework.run(&workspace);

			} else {
#endif
				// we can only handle one file
				if (files.size() != 1) {
					OtLogFatal("Error: you can only run one file, you specified {}", files.size());
				}

				// run the file
				auto file = files[0];
				auto extension = OtPathGetExtension(file);

				// execute by type (based by the file extension)
				if (extension == ".ot" || extension == "") {
					// compile and run script as a module
					auto module = OtModule::create();
					module->load(file);
					module->unsetAll();

#if defined(INCLUDE_GUI)
				} else if (extension == ".ots") {
					// handle a scene file
					OtFramework framework;
					OtSceneApp app{file};
					framework.run(&app);
#endif

				} else {
					OtLogFatal("Error: can't execute file with extension [{}]", extension);
				}
#if defined(INCLUDE_GUI)
			}
#endif
		}

		// cleanup
		OtLibUv::end();

	} catch (OtException& e) {
		// send exception back to IDE (if required)
		OtLogger::instance()->exception(e);

		// output human readable text
		OtLogFatal("Error: {}", e.what());
	}

	return 0;
}
