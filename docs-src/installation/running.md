Once you have compiled ObjectTalk, you end up with a
single executable the is either in the **debug/bin**,
**debug/release**, **debug/xcode** or **debug/vs**
directories depending on the platform or the method used.
This executable can be moved anywhere and in the future
is will be distributed in a package suitable for the
platform.

In priciple, the ObjectTalk executable (**ot**) is a
command line tool that take parameters. Here is the
help you get when you run **ot -h**:

	Usage: ot [options] scripts

	Positional arguments:
	scripts      	scripts to execute

	Optional arguments:
	-h --help    	shows help message and exits
	-v --version 	prints version information and exits
	-i --ide     	edit code instead of running it [default: false]

So for example:

* Just running the executable without parameters launches
an Integrated Development Environment (IDE) that allow
you to edit ObjectTalk scripts, run them as see possible
errors.
* You can directly run an ObjectTalk script using
**ot [script name]**
* you can also open a sccript directly in the IDE by
using **ot -i [script name]**
