The ObjectTalk package is written in C++ and can be
compiled on MacOS, Linux and Windows. To simplify the build process,
[CMake](https://cmake.org) is used to deal with
cross-platform challenges. Right now, all platforms
support the build process from the command line using
very simple instructions. MacOS and Windows also support
using visual development environments like Xcode and
Visual Studio (Code).

ObjectTalk uses a large number of 3rd party
Open Source packages. The CMake process automatically
loads the correct versions, compiles them and links
them into the ObjectTalk executable (simply named **ot**).
No endless downloads, frustrations over each package's
peculiarities or countless Google searches. The plan
is to just support it out of the box.

Below are simple instructions for each of the supported
platforms followed by some hints that apply to all
platforms.
