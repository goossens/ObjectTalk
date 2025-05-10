//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstring>
#include <string>

#ifndef _WIN32
#include <signal.h>
#endif

#include <argparse/argparse.hpp>

#include "OtConfig.h"
#include "OtException.h"
#include "OtLibuv.h"
#include "OtPath.h"
#include "OtStderrMultiplexer.h"
#include "OtModule.h"

#if defined(OT_INCLUDE_UI)
#include "OtFramework.h"
#include "OtSceneApp.h"
#include "OtWorkspace.h"
#endif


//
//	ObjectTalk main function
//

//
//	Startup logic
//	=============
//
//	If compiled with UI
//		If one file is specified on the command line
//			do the following based on file extension
//				.ot  - compile and run script
//				.ots - run scene file
//				.otn - open nodes in IDE
//				.* - open file in IDE
//
//		Elseif more than one files is specified on the command line
//			Open all files in the IDE
//
//		Elseif no files are specified on the command line
//			Start IDE
//
//	Else
//		If one file is specified on the command line
//			If file is .ot
//				compile and run script
//
//			Else
//				Exit with error
//
//		Else
//			Exit with error
//


int main(int argc, char* argv[]) {
#ifndef _WIN32
	// disable signal handler for SIGPIPE on UNIX type systems
	// we handle EPIPE errors instead
	signal(SIGPIPE, SIG_IGN);
#endif

	// parse all command line parameters
	argparse::ArgumentParser program(argv[0], "0.4");
	bool childProcessFlag = false;
	std::string logFile;

	program.add_argument("-c", "--child")
		.help("run as an IDE child process")
		.store_into(childProcessFlag);

	program.add_argument("-l", "--log")
		.help("specify a file to send log to")
		.metavar("filename")
		.store_into(logFile);

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

	// set configuration
	OtConfig::setSubprocessMode(childProcessFlag);

	// log to file (if required)
	if (logFile.size()) {
		OtLog::setFileLogging(logFile);
	}

	try {
		// initialize libuv
		OtLibUv::init(argc, argv);

#if defined(OT_INCLUDE_UI)
		//
		// UI conguration
		//

		if (files.size() == 1) {
				// handle case of single file on command line
				auto file = files[0];
				auto extension = OtPath::getExtension(file);

				// check file extension
				if (extension == ".ot" || extension == "") {
					// compile and run script as a module
					auto module = OtModule::create();
					module->load(file);
					module->unsetAll();

				} else if (extension == ".ots") {
					// handle a scene file
					OtFramework framework;
					OtSceneApp app{file};
					framework.run(&app);

				} else {
					// handle other file types
					OtFramework framework;
					OtWorkspace workspace;
					workspace.openFile(file, OtEditor::VisualState::inTab);
					framework.run(&workspace);
				}

		} else if (files.size() > 1) {
			// handle case of multiple files on the command line
			OtFramework framework;
			OtWorkspace workspace;

			for (auto& file : files) {
				workspace.openFile(file, OtEditor::VisualState::inTab);
			}

			framework.run(&workspace);

		} else {
			// handle case of no files specified on command line
			OtFramework framework;
			OtWorkspace workspace;
			framework.run(&workspace);
		}

#else
		//
		// Non-UI configuration
		//

		// were any files specified?
		if (files.size() == 0) {
			OtLogFatal("No files specified");

		} else {
			// we can only handle one file
			if (files.size() != 1) {
				OtLogFatal("Error: you can only run one file, you specified {}", files.size());
			}

			// run the file
			auto file = files[0];
			auto extension = OtPath::getExtension(file);

			if (extension == ".ot") {
				// compile and run script as a module
				auto module = OtModule::create();
				module->load(file);
				module->unsetAll();

			} else {
				OtLogFatal("Error: can't execute file with extension [{}]", extension);
			}
		}
#endif

		// cleanup
		OtLibUv::end();

	} catch (OtException& e) {
		// send exception back to IDE (if required)
		if (OtConfig::inSubprocessMode()) {
			OtStderrMultiplexer::multiplex(e);
		}

		// output human readable error message
		OtLogFatal("Error: {}", e.what());
	}

	return 0;
}
