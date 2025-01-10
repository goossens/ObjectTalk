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

#include <argparse/argparse.hpp>

#include "OtConfig.h"
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
//			if MacOS and openFile(s) message is sent from the Finder
//				Restart program with files as command line parameters
//
//			Else
//				Start IDE
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
	// parse all command line parameters
	argparse::ArgumentParser program(argv[0], "0.3");
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
					framework.initialize();
					framework.run(&app);
					framework.terminate();

				} else {
					// handle other file types
					OtFramework framework;
					framework.initialize();

					// open in IDE
					OtWorkspace workspace;
					workspace.openFile(file, OtEditor::VisualState::inTab);
					framework.run(&workspace);
					framework.terminate();
				}

		} else if (files.size() > 1) {
			// handle case of multiple files on the command line
			OtFramework framework;
			framework.initialize();

			// open each file in the IDE
			OtWorkspace workspace;

			for (auto& file : files) {
				workspace.openFile(file, OtEditor::VisualState::inTab);
			}

			framework.run(&workspace);
			framework.terminate();

		} else {
			// handle case of no files specified on command line
			OtFramework framework;
			framework.initialize();

			// see if we have any files passed by the Finder (this only applies to MacOS)
			for (auto& filename : OtFramework::getStartupFiles()) {
				files.emplace_back(filename);
			}

			// do we have any files now?
			if (files.size()) {
				// yes, relaunch and specify files on command line
				char** args = new char* [files.size() + 2];

				// create argument array
				auto executable = OtPath::getExecutable();
				args[0] = (char*) executable.c_str();
				int i = 1;

				for (auto& file : files) {
					args[i++] = (char*) file.c_str();
				}

				args[i] = nullptr;

				// setup process options
				uv_process_t subprocess;

				uv_stdio_container_t stdio[3];
				stdio[0].flags = UV_IGNORE;
				stdio[1].flags = UV_IGNORE;
				stdio[2].flags = UV_IGNORE;

				uv_process_options_t options;
				memset(&options, 0, sizeof(options));
				options.file = args[0];
				options.args = args;
				options.flags = UV_PROCESS_DETACHED | UV_PROCESS_WINDOWS_HIDE_CONSOLE;
				options.stdio_count = 3;
				options.stdio = stdio;

				// start process
				auto status = uv_spawn(uv_default_loop(), &subprocess, &options);

				if (status) {
					delete [] args;
					UV_CHECK_ERROR2("uv_spawn", status, executable.c_str());
				}

				uv_unref((uv_handle_t*) &subprocess);
				delete [] args;

			} else {
				OtWorkspace workspace;
				framework.run(&workspace);
			}

			framework.terminate();
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
