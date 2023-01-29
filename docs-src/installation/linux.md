ObjectTalk is supported on Linux. Given the large number
of flavors, the core team only tests the build process
on Alpine Linux and Ubuntu 22.04 Desktop. The Alpine
environment is used to test a headless version (just
a command line tool without a GUI) and the Ubuntu version
is used to test the GUI version. In practice, any
combination should work as it just depends on the
availability of X-Windows and OpenGL. Other flavors
of Linux will likely work as well but we'll let the
community provide the instructions.

The headless versions can be easily tested with Docker.
The **docker** folder contains the recipes and you can
quickly spawn a docker container by doing a
**make alpine** or **make ubuntu** from the top-level
directory. You obviously have to have Docker and make
installed on your host platform. The developers use
this frequently on a MacOS platform.

To build the full version of ObjectTalk on Ubuntu 22.04
Desktop with GUI and IDE, run the following commands:

* Install additional software using **sudo apt-get install build-essential cmake git xorg-dev**
* **cd [to your favorite build directory]**
* If your favorite directory is on a mounted drive, you might have to do a **git config --global --replace-all safe.directory '*'**
* **git clone https://github.com/goossens/ObjectTalk.git**
* **cd ObjectTalk**
* Create a debug version with **make debug** or create a release version with **make release**
* You can run the test suite for the language with **make test**
* The created binary will be **./debug/bin/ot** or **./release/bin/ot** depending on the build type

If you don't want the GUI, look at the Dockerfiles to see
what packages are required.
