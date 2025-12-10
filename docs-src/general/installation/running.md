Once you have compiled ObjectTalk, you end up with a
single executable in one of the build directories
depending on the platform or the method used.
This executable can be moved anywhere.

* Executable locations:
	* **Command line (MacOS Debug)**: ${HOME}/build/ObjectTalk/Debug/ot
	* **Command line (MacOS Release)**: ${HOME}/build/ObjectTalk/Release/ot
	* **Xcode**: ${HOME}/build/ObjectTalk/xcode/Debug/ot
	* **Command line (Linux Debug)**: ${HOME}/build/ObjectTalk/Debug/ot
	* **Command line (Linux Release)**: ${HOME}/build/ObjectTalk/Release/ot
	* **Windows (Debug)**: ${HOME}/build/ObjectTalk/vs/Debug/ot.exe
	* **Windows (Release)**: ${HOME}/build/ObjectTalk/vs/Release/ot.exe

In principle, the ObjectTalk executable (**ot**) is a
command line tool that takes parameters. Here is the
help you get when you run **ot -h**:

	Usage: ot [--help] [--version] [--child] [--log filename] [files]...

	Positional arguments:
	files               files to process [nargs: 0 or more]

	Optional arguments:
	-h --help			shows help message and exits
	-v --version		prints version information and exits
	-c, --child         run as an IDE child process (internal use only)
	-l, --log filename  specify a file to send log to

Startup logic:

	If compiled with UI
		If one file is specified on the command line
			do the following based on file extension
				.ot  - compile and run script
				.ots - run scene file
				.otn - open nodes in IDE
				.* - open file in IDE

		Elseif more than one files is specified on the command line
			Open all files in the IDE

		Elseif no files are specified on the command line
			Start IDE

		Else
		If one file is specified on the command line
			If file is .ot
				compile and run script

			Else
				Exit with error

		Else
			Exit with error
